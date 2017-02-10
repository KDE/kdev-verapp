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

#include "projectconfigpage.h"
#include "ui_projectconfigpage.h"

#include "parameters.h"
#include "projectsettings.h"
#include "rules.h"

#include <interfaces/iproject.h>

namespace verapp
{

ProjectConfigPage::ProjectConfigPage(KDevelop::IPlugin* plugin, KDevelop::IProject* project, QWidget* parent)
    : ConfigPage(plugin, new ProjectSettings, parent)
    , ui(new Ui::ProjectConfigPage)
    , m_parameters(new Parameters(project))
{
    configSkeleton()->setSharedConfig(project->projectConfiguration());
    configSkeleton()->load();

    ui->setupUi(this);

    ui->commandLine->setFontFamily("Monospace");

    connect(this, &ProjectConfigPage::changed, this, &ProjectConfigPage::updateCommandLine);
    connect(ui->commandLineFilter, &QLineEdit::textChanged, this, &ProjectConfigPage::updateCommandLine);
    connect(ui->commandLineBreaks, &QCheckBox::stateChanged, this, &ProjectConfigPage::updateCommandLine);

    addRules(ui->tabF,  rules::F001, rules::F002);
    addRules(ui->tabL,  rules::L001, rules::L006);
    addRules(ui->tabT1, rules::T001, rules::T007);
    addRules(ui->tabT2, rules::T008, rules::T014);
    addRules(ui->tabT3, rules::T015, rules::T019);
}

ProjectConfigPage::~ProjectConfigPage()
{
}

QIcon ProjectConfigPage::icon() const
{
   return QIcon::fromTheme(QStringLiteral("dialog-ok"));
}

void ProjectConfigPage::defaults()
{
    ConfigPage::defaults();
    updateCommandLine();
}

void ProjectConfigPage::reset()
{
    ConfigPage::reset();
    updateCommandLine();
}

QString ProjectConfigPage::name() const
{
    return i18n("Vera++");
}

void ProjectConfigPage::updateCommandLine()
{
    for (int intType = rules::Type::FIRST; intType <= rules::Type::LAST; ++intType) {
        rules::Type ruleType = static_cast<rules::Type>(intType);

        m_parameters->setRuleEnabled(ruleType, m_rulesCheckBox[ruleType]->isChecked());
    }

    m_parameters->extraParameters = ui->kcfg_extraParameters->text().trimmed();

    QString text = m_parameters->commandLine().join(' ');
    if (!ui->commandLineBreaks->isChecked()) {
        ui->commandLine->setText(text);
        return;
    }

    text.replace(" -", "\n-");
    if (ui->commandLineFilter->text().isEmpty()) {
        ui->commandLine->setText(text);
        ui->commandLineBreaks->setEnabled(true);
        return;
    }

    QStringList lines = text.split('\n');
    QMutableStringListIterator i(lines);

    while (i.hasNext()) {
        if (!i.next().contains(ui->commandLineFilter->text())) {
            i.remove();
        }
    }

    ui->commandLine->setText(lines.join('\n'));
    ui->commandLineBreaks->setEnabled(false);
}

void ProjectConfigPage::addRules(QWidget* tab, rules::Type first, rules::Type last)
{
    QVBoxLayout* layout = new QVBoxLayout(tab);

    for (int intType = first; intType <= last; ++intType) {
        rules::Type type = static_cast<rules::Type>(intType);
        QCheckBox* checkBox = new QCheckBox(tab);

        checkBox->setObjectName(QString("kcfg_rule%1").arg(intType));
        checkBox->setText(QString("%1: %2").arg(rules::name(type)).arg(rules::title(type)));
        checkBox->setToolTip(rules::explanation(type));

        layout->addWidget(checkBox);

        m_rulesCheckBox[type] = checkBox;
    }
    layout->addStretch();
}

}
