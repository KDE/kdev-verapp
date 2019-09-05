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

#ifndef VERAPP_PARAMETERS_H
#define VERAPP_PARAMETERS_H

#include "rules.h"

#include <util/path.h>

namespace KDevelop
{
class IProject;
}

namespace verapp
{

namespace defaults
{

QString executablePath();
bool hideOutputView();

bool isRuleEnabled(rules::Type type);

}

class Parameters
{

public:
    explicit Parameters(KDevelop::IProject* project = nullptr);

    QStringList commandLine() const;

    QString buildRunScript() const;

    // global settings
    QString executablePath;
    bool hideOutputView;

    // project settings
    bool isRuleEnabled(rules::Type type) const;
    void setRuleEnabled(rules::Type type, bool value);

    QString extraParameters;

    QString checkPath;

private:
    QString applyPlaceholders(const QString& text) const;

    KDevelop::IProject* m_project;

    KDevelop::Path m_projectRootPath;
    KDevelop::Path m_projectBuildPath;

    bool m_ruleEnabled[rules::Type::COUNT];
};

}
#endif
