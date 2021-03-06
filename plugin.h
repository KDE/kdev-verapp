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

#ifndef VERAPP_PLUGIN_H
#define VERAPP_PLUGIN_H

#include "job.h"

#include <interfaces/iplugin.h>

class KJob;
class QMenu;

namespace KDevelop
{
    class IProject;
}

namespace verapp
{

class ProblemModel;

class Plugin : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    Plugin(QObject* parent, const QVariantList& = QVariantList());

    ~Plugin() override;

    int configPages() const override { return 1; }
    KDevelop::ConfigPage* configPage(int number, QWidget* parent) override;

    int perProjectConfigPages() const override { return 1; }
    KDevelop::ConfigPage* perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions& options, QWidget* parent) override;

    KDevelop::ContextMenuExtension contextMenuExtension(KDevelop::Context* context, QWidget* parent) override;

    void runVerapp(KDevelop::IProject* project, const QString& path);
    bool isRunning();

private:
    void killVerapp();

    void raiseProblemsView();
    void raiseOutputView();

    void updateActions();
    void projectClosed(KDevelop::IProject* project);

    void runVerapp(bool checkProject);

    void result(KJob* job);

    Job* m_job;

    KDevelop::IProject* m_project;

    QAction* m_menuActionFile;
    QAction* m_menuActionProject;
    QAction* m_contextActionFile;
    QAction* m_contextActionProject;
    QAction* m_contextActionProjectItem;

    QScopedPointer<ProblemModel> m_model;
};

}

#endif
