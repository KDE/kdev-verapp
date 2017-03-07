/* This file is part of KDevelop

   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "parameters.h"

#include "globalsettings.h"
#include "projectsettings.h"
#include "rules.h"

#include <KShell>
#include <klocalizedstring.h>
#include <interfaces/iproject.h>
#include <serialization/indexedstring.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <project/projectmodel.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>
#include <QMessageBox>

namespace verapp
{

namespace defaults
{

QString executablePath()
{
    QString path = QStandardPaths::findExecutable(QStringLiteral("vera++"));
    return path.isEmpty() ? QStringLiteral("vera++") : path;
}

}

Parameters::Parameters(KDevelop::IProject* project)
    : m_project(project)
{
    executablePath = KDevelop::Path(GlobalSettings::executablePath()).toLocalFile();
    hideOutputView = GlobalSettings::hideOutputView();

    if (!project) {
        for (int intType = rules::Type::FIRST; intType <= rules::Type::LAST; ++intType) {
            rules::Type ruleType = static_cast<rules::Type>(intType);

            m_ruleEnabled[ruleType] = defaults::isRuleEnabled(ruleType);
        }

        fileFilter = defaults::fileFilter;
        return;
    }

    ProjectSettings projectSettings;
    projectSettings.setSharedConfig(project->projectConfiguration());
    projectSettings.load();

    for (int intType = rules::Type::FIRST; intType <= rules::Type::LAST; ++intType) {
        rules::Type ruleType = static_cast<rules::Type>(intType);

        m_ruleEnabled[ruleType] = projectSettings.rule(intType);
    }

    extraParameters = projectSettings.extraParameters();
    fileFilter = projectSettings.fileFilter();

    m_projectRootPath  = m_project->path();
    m_projectBuildPath = m_project->buildSystemManager()->buildDirectory(m_project->projectItem());
}

QStringList Parameters::commandLine() const
{
    QStringList arguments;

    arguments << executablePath;

    arguments << QStringLiteral("--show-rule");
    arguments << QStringLiteral("--no-duplicate");

    for (int intType = rules::Type::FIRST; intType <= rules::Type::LAST; ++intType) {
        rules::Type ruleType = static_cast<rules::Type>(intType);

        if (m_ruleEnabled[ruleType]) {
            arguments << "-R";
            arguments << rules::name(ruleType);
        }
    }

    if (!extraParameters.isEmpty()) {
        arguments << KShell::splitArgs(applyPlaceholders(extraParameters));
    }

    return arguments;
}

QString Parameters::buildRunScript() const
{
    QSet<KDevelop::IndexedString> projectFiles;
    QString scriptPath;

    if (!checkPath.isEmpty() && QFileInfo(checkPath).isFile()) {
        projectFiles.insert(KDevelop::IndexedString(checkPath));
    } else if (m_project) {
        projectFiles = m_project->fileSet();
        QStringList filters = fileFilter.split(',', QString::SkipEmptyParts);

        QMutableSetIterator<KDevelop::IndexedString> i(projectFiles);
        while (i.hasNext()) {
            QString projectFile = i.next().str();

            if (!projectFile.startsWith(checkPath)) {
                i.remove();
                continue;
            }

            if (!QDir::match(filters, projectFile)) {
                i.remove();
            }
        }
    }

    QString tempDir = QStandardPaths::standardLocations(QStandardPaths::TempLocation).first();
    scriptPath = tempDir + "/kdevverapp";

    if (m_project) {
        scriptPath += "_" + m_project->name();
    }

#ifdef Q_OS_WIN
    scriptPath += ".bat";
#else
    scriptPath += ".sh";
#endif

    QFile scriptFile(scriptPath);
    if (!scriptFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(
            nullptr,
            i18n("Vera++ Error"),
            i18n("Unable to open file '%1' to write", scriptPath));

        return scriptPath;
    }

    QTextStream scriptStream(&scriptFile);
    QString veraCommand = commandLine().join(' ');
    int fileNumber = 0;
    int filesCount = projectFiles.size();
    foreach (const KDevelop::IndexedString& projectFile, projectFiles) {
        scriptStream << QString("echo Checking %1 ...\n").arg(projectFile.str());

        scriptStream << QString("%1 %2\n").arg(veraCommand).arg(projectFile.str());

        int progress = (++fileNumber)/(double)filesCount * 100.0;
        scriptStream << QString("echo %1/%2 files checked %3% done\n").arg(fileNumber).arg(filesCount).arg(progress);
    }

    scriptFile.setPermissions(
        QFileDevice::ReadOwner |
        QFileDevice::WriteOwner |
        QFileDevice::ExeOwner);
    scriptFile.close();

    return scriptPath;
}

bool Parameters::isRuleEnabled(rules::Type type) const
{
    return m_ruleEnabled[type];
}

void Parameters::setRuleEnabled(rules::Type type, bool value)
{
    m_ruleEnabled[type] = value;
}

QString Parameters::applyPlaceholders(const QString& text) const
{
    QString result(text);

    if (m_project) {
        result.replace("%p", m_projectRootPath.toLocalFile());
        result.replace("%b", m_projectBuildPath.toLocalFile());
    }

    return result;
}

namespace defaults
{

bool isRuleEnabled(rules::Type type)
{
    // default Vera++ profile
    if (type == rules::F002 || type == rules::T014) {
        return false;
    }

    return true;
}

}

}
