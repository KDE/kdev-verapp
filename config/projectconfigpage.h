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

#ifndef VERAPP_PROJECT_CONFIG_PAGE_H
#define VERAPP_PROJECT_CONFIG_PAGE_H

#include "rules.h"

#include <interfaces/configpage.h>

#include <QCheckBox>

namespace KDevelop
{
class IProject;
}

namespace verapp
{

namespace Ui
{
class ProjectConfigPage;
}

class Parameters;

class ProjectConfigPage : public KDevelop::ConfigPage
{
    Q_OBJECT

public:

    ProjectConfigPage(KDevelop::IPlugin* plugin, KDevelop::IProject* project, QWidget* parent);
    ~ProjectConfigPage() override;

    QIcon icon() const override;
    QString name() const override;

public slots:
    void defaults() override;
    void reset() override;

private:
    void updateCommandLine();
    void addRules(QWidget* tab, rules::Type first, rules::Type last);

    QScopedPointer<Ui::ProjectConfigPage> ui;
    QScopedPointer<Parameters> m_parameters;

    QHash<rules::Type, QCheckBox*> m_rulesCheckBox;
};

}

#endif
