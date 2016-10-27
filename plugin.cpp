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

#include <interfaces/contextmenuextension.h>
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/iuicontroller.h>
#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <language/interfaces/editorcontext.h>
#include <project/projectconfigpage.h>
#include <project/projectmodel.h>
#include <shell/problemmodel.h>
#include <shell/problemmodelset.h>
#include <util/jobstatus.h>

#include "rules.h"

K_PLUGIN_FACTORY_WITH_JSON(VerappFactory, "kdevverapp.json", registerPlugin<verapp::Plugin>();)

namespace verapp
{

static const QString modelName("Vera++");

Plugin::Plugin(QObject* parent, const QVariantList&)
    : IPlugin("kdevverapp", parent)
    , m_job(nullptr)
    , m_currentProject(nullptr)
    , m_checkedProject(nullptr)
    , m_model(new KDevelop::ProblemModel(this))
{
    qCDebug(KDEV_VERAPP) << "setting kdevverapp.rc file";
    setXMLFile("kdevverapp.rc");

    rules::init();

    m_actionFile = new QAction(i18n("Vera++ (Current File)"), this);
    connect(m_actionFile, &QAction::triggered, [this](){
        runVerapp(false);
    });
    actionCollection()->addAction("verapp_file", m_actionFile);

    m_actionProject = new QAction(i18n("Vera++ (Current Project)"), this);
    connect(m_actionProject, &QAction::triggered, [this](){
        runVerapp(true);
    });
    actionCollection()->addAction("verapp_project", m_actionProject);

    m_actionProjectItem = new QAction("Vera++", this);

    connect(core()->documentController(), &KDevelop::IDocumentController::documentClosed,
            this, &Plugin::updateActions);
    connect(core()->documentController(), &KDevelop::IDocumentController::documentActivated,
            this, &Plugin::updateActions);

    connect(core()->projectController(), &KDevelop::IProjectController::projectOpened,
            this, &Plugin::updateActions);
    connect(core()->projectController(), &KDevelop::IProjectController::projectClosed,
            this, &Plugin::projectClosed);

    KDevelop::ProblemModelSet* pms = core()->languageController()->problemModelSet();
    pms->addModel(modelName, m_model.data());

    updateActions();
}

Plugin::~Plugin()
{
    killVerapp();

    KDevelop::ProblemModelSet* pms = core()->languageController()->problemModelSet();
    pms->removeModel(modelName);
}

bool Plugin::isRunning()
{
    return m_job;
}

void Plugin::killVerapp()
{
    if (m_job)
        m_job->kill(KJob::EmitResult);
}

void Plugin::raiseProblemsView()
{
    core()->languageController()->problemModelSet()->showModel(modelName);
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
    m_currentProject = nullptr;

    m_actionFile->setEnabled(false);
    m_actionProject->setEnabled(false);

    if (isRunning())
        return;

    KDevelop::IDocument* activeDocument = core()->documentController()->activeDocument();
    if (!activeDocument)
        return;

    QUrl url = activeDocument->url();

    m_currentProject = core()->projectController()->findProjectForUrl(url);
    if (!m_currentProject)
        return;

    m_actionFile->setEnabled(true);
    m_actionProject->setEnabled(true);
}

void Plugin::projectClosed(KDevelop::IProject* project)
{
    if (project != m_checkedProject)
        return;

    killVerapp();
    m_problems.clear();
    m_model->clearProblems();
    m_checkedProject = nullptr;
}

void Plugin::runVerapp(bool checkProject)
{
    KDevelop::IDocument* doc = core()->documentController()->activeDocument();
    Q_ASSERT(doc);

    if (checkProject)
        runVerapp(m_currentProject, m_currentProject->path().toUrl().toLocalFile());
    else
        runVerapp(m_currentProject, doc->url().toLocalFile());
}

void Plugin::runVerapp(KDevelop::IProject* project, const QString& path)
{
    m_checkedProject = project;

    Parameters params(m_checkedProject);
    params.checkPath = path;

    m_problems.clear();
    m_model->clearProblems();

    m_job = new Job(params);

    connect(m_job, &Job::problemsDetected, this, &Plugin::problemsDetected);
    connect(m_job, &Job::finished,         this, &Plugin::result);

    core()->uiController()->registerStatus(new KDevelop::JobStatus(m_job, "vera++"));
    core()->runController()->registerJob(m_job);

    if (params.hideOutputView)
        raiseProblemsView();
    else
        raiseOutputView();

    updateActions();
}

void Plugin::problemsDetected(const QVector<KDevelop::IProblem::Ptr>& problems)
{
    static int maxLength = 0;

    if (m_problems.isEmpty())
        maxLength = 0;

    m_problems.append(problems);
    for (auto p : problems) {
        m_model->addProblem(p);

        // this performs adjusing of columns width in the ProblemsView
        if (maxLength < p->description().length()) {
            maxLength = p->description().length();
            m_model->setProblems(m_problems);
        }
    }
}

void Plugin::result(KJob*)
{
    if (!core()->projectController()->projects().contains(m_checkedProject)) {
        m_problems.clear();
        m_model->clearProblems();
    } else {
        m_model->setProblems(m_problems);

        if (m_job->status() == KDevelop::OutputExecuteJob::JobStatus::JobSucceeded ||
            m_job->status() == KDevelop::OutputExecuteJob::JobStatus::JobCanceled)
            raiseProblemsView();
        else
            raiseOutputView();
    }

    m_job = nullptr; // job is automatically deleted later

    updateActions();
}

KDevelop::ContextMenuExtension Plugin::contextMenuExtension(KDevelop::Context* context)
{
    KDevelop::ContextMenuExtension extension;

    if (context->hasType(KDevelop::Context::EditorContext) &&
        m_currentProject && !isRunning()) {

            extension.addAction(KDevelop::ContextMenuExtension::AnalyzeGroup, m_actionFile);
            extension.addAction(KDevelop::ContextMenuExtension::AnalyzeGroup, m_actionProject);
    }

    if (context->hasType(KDevelop::Context::ProjectItemContext) && !isRunning()) {
        auto pContext = dynamic_cast<KDevelop::ProjectItemContext*>(context);
        if (pContext->items().size() != 1)
            return extension;

        auto item = pContext->items().first();

        switch (item->type()) {
            case KDevelop::ProjectBaseItem::File:
            case KDevelop::ProjectBaseItem::Folder:
            case KDevelop::ProjectBaseItem::BuildFolder:
                break;

            default:
                return extension;
        }

        m_actionProjectItem->disconnect();
        connect(m_actionProjectItem, &QAction::triggered, [this, item](){
            runVerapp(item->project(), item->path().toLocalFile());
        });

        extension.addAction(KDevelop::ContextMenuExtension::AnalyzeGroup, m_actionProjectItem);
    }

    return extension;
}

KDevelop::ConfigPage* Plugin::perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions& options, QWidget* parent)
{
    if (number != 0)
        return nullptr;
    else
        return new ProjectConfigPage(this, options.project, parent);
}

KDevelop::ConfigPage* Plugin::configPage(int number, QWidget* parent)
{
    if (number != 0)
        return nullptr;
    else
        return new GlobalConfigPage(this, parent);
}

}

#include "plugin.moc"
