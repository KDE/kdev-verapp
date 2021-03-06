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

#include "plugin.h"

#include "config/globalconfigpage.h"
#include "config/projectconfigpage.h"
#include "debug.h"
#include "problemmodel.h"
#include "rules.h"
#include "utils.h"

#include <interfaces/contextmenuextension.h>
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/iuicontroller.h>
#include <kactioncollection.h>
#include <kmessagebox.h>
#include <kpluginfactory.h>
#include <language/interfaces/editorcontext.h>
#include <project/projectconfigpage.h>
#include <project/projectmodel.h>
#include <util/jobstatus.h>

#include <QApplication>
#include <QFile>

K_PLUGIN_FACTORY_WITH_JSON(VerappFactory, "kdevverapp.json", registerPlugin<verapp::Plugin>();)

namespace verapp
{

Plugin::Plugin(QObject* parent, const QVariantList&)
    : IPlugin("kdevverapp", parent)
    , m_job(nullptr)
    , m_project(nullptr)
    , m_model(new ProblemModel(this))
{
    qCDebug(KDEV_VERAPP) << "setting kdevverapp.rc file";
    setXMLFile("kdevverapp.rc");

    rules::init();

    m_menuActionFile = new QAction(i18n("Analyze Current File with Vera++"), this);
    connect(m_menuActionFile, &QAction::triggered, [this](){
        runVerapp(false);
    });
    actionCollection()->addAction("verapp_file", m_menuActionFile);

    m_contextActionFile = new QAction(i18n("Vera++"), this);
    connect(m_contextActionFile, &QAction::triggered, [this]() {
        runVerapp(false);
    });

    m_menuActionProject = new QAction(i18n("Analyze Current Project with Vera++"), this);
    connect(m_menuActionProject, &QAction::triggered, [this](){
        runVerapp(true);
    });
    actionCollection()->addAction("verapp_project", m_menuActionProject);

    m_contextActionProject = new QAction(i18n("Vera++"), this);
    connect(m_contextActionProject, &QAction::triggered, [this]() {
        runVerapp(true);
    });

    m_contextActionProjectItem = new QAction("Vera++", this);

    connect(core()->documentController(), &KDevelop::IDocumentController::documentClosed,
            this, &Plugin::updateActions);
    connect(core()->documentController(), &KDevelop::IDocumentController::documentActivated,
            this, &Plugin::updateActions);

    connect(core()->projectController(), &KDevelop::IProjectController::projectOpened,
            this, &Plugin::updateActions);
    connect(core()->projectController(), &KDevelop::IProjectController::projectClosed,
            this, &Plugin::projectClosed);

    updateActions();
}

Plugin::~Plugin()
{
    killVerapp();
}

bool Plugin::isRunning()
{
    return m_job;
}

void Plugin::killVerapp()
{
    if (m_job) {
        m_job->kill(KJob::EmitResult);
    }
}

void Plugin::raiseProblemsView()
{
    m_model->show();
}

void Plugin::raiseOutputView()
{
    core()->uiController()->findToolView(
        i18ndc("kdevstandardoutputview", "@title:window", "Test"),
        nullptr,
        KDevelop::IUiController::FindFlags::Raise);
}

void Plugin::updateActions()
{
    m_project = nullptr;

    m_menuActionFile->setEnabled(false);
    m_menuActionProject->setEnabled(false);

    if (isRunning()) {
        return;
    }

    auto activeDocument = core()->documentController()->activeDocument();
    if (!activeDocument) {
        return;
    }

    m_project = core()->projectController()->findProjectForUrl(activeDocument->url());
    if (!m_project) {
        return;
    }

    m_menuActionFile->setEnabled(true);
    m_menuActionProject->setEnabled(true);
}

void Plugin::projectClosed(KDevelop::IProject* project)
{
    if (project != m_model->project()) {
        return;
    }

    killVerapp();
    m_model->reset();
}

void Plugin::runVerapp(bool checkProject)
{
    auto doc = core()->documentController()->activeDocument();
    Q_ASSERT(doc);

    if (checkProject) {
        runVerapp(m_project, m_project->path().toUrl().toLocalFile());
    } else {
        runVerapp(m_project, doc->url().toLocalFile());
    }
}

void Plugin::runVerapp(KDevelop::IProject* project, const QString& path)
{
    Parameters params(project);
    params.checkPath = path;

    m_model->reset(project, path);

    if (!QFile::exists(params.executablePath)) {
        QString errorMessage;
        errorMessage += i18n("Failed to start vera++ from \"%1\".", params.executablePath);
        errorMessage += QStringLiteral("\n\n");
        errorMessage += i18n("Check your settings and install the vera++ if necessary.");

        KMessageBox::error(qApp->activeWindow(), errorMessage, i18n("Vera++ Error"));
        return;
    }

    m_job = new Job(params);

    connect(m_job, &Job::problemsDetected, m_model.data(), &ProblemModel::addProblems);
    connect(m_job, &Job::finished, this, &Plugin::result);

    core()->uiController()->registerStatus(new KDevelop::JobStatus(m_job, "vera++"));
    core()->runController()->registerJob(m_job);

    if (params.hideOutputView) {
        raiseProblemsView();
    } else {
        raiseOutputView();
    }

    updateActions();
}

void Plugin::result(KJob*)
{
    if (!core()->projectController()->projects().contains(m_model->project())) {
        m_model->reset();
    } else {
        m_model->setProblems();

        if (m_job->status() == KDevelop::OutputExecuteJob::JobStatus::JobSucceeded ||
            m_job->status() == KDevelop::OutputExecuteJob::JobStatus::JobCanceled) {

            raiseProblemsView();
        } else {
            raiseOutputView();
        }
    }

    m_job = nullptr; // job is automatically deleted later

    updateActions();
}

KDevelop::ContextMenuExtension Plugin::contextMenuExtension(KDevelop::Context* context, QWidget* parent)
{
    Q_UNUSED(parent);
    KDevelop::ContextMenuExtension extension;

    if (context->hasType(KDevelop::Context::EditorContext) && m_project && !isRunning()) {
        auto eContext = static_cast<KDevelop::EditorContext*>(context);
        if (isSupportedUrl(eContext->url()))
        {
            extension.addAction(KDevelop::ContextMenuExtension::AnalyzeFileGroup, m_contextActionFile);
        }

        extension.addAction(KDevelop::ContextMenuExtension::AnalyzeProjectGroup, m_contextActionProject);
    }

    if (context->hasType(KDevelop::Context::ProjectItemContext) && !isRunning()) {
        auto pContext = dynamic_cast<KDevelop::ProjectItemContext*>(context);
        if (pContext->items().size() != 1) {
            return extension;
        }

        auto item = pContext->items().first();

        switch (item->type()) {
            case KDevelop::ProjectBaseItem::File: {
                if (!isSupportedUrl(item->path().toUrl())) {
                    return extension;
                }
                break;
            }
            case KDevelop::ProjectBaseItem::Folder:
            case KDevelop::ProjectBaseItem::BuildFolder:
                break;

            default:
                return extension;
        }

        m_contextActionProjectItem->disconnect();
        connect(m_contextActionProjectItem, &QAction::triggered, [this, item](){
            runVerapp(item->project(), item->path().toLocalFile());
        });

        extension.addAction(KDevelop::ContextMenuExtension::AnalyzeProjectGroup, m_contextActionProjectItem);
    }

    return extension;
}

KDevelop::ConfigPage* Plugin::perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions& options, QWidget* parent)
{
    if (number) {
        return nullptr;
    }

    return new ProjectConfigPage(this, options.project, parent);
}

KDevelop::ConfigPage* Plugin::configPage(int number, QWidget* parent)
{
    if (number) {
        return nullptr;
    }

    return new GlobalConfigPage(this, parent);
}

}

#include "plugin.moc"
