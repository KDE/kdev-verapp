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

#ifndef VERAPP_RULES_H
#define VERAPP_RULES_H

#include <QString>

namespace verapp
{

namespace rules
{
    enum Type
    {
        F001,
        F002,

        L001,
        L002,
        L003,
        L004,
        L005,
        L006,

        T001,
        T002,
        T003,
        T004,
        T005,
        T006,
        T007,
        T008,
        T009,
        T010,
        T011,
        T012,
        T013,
        T014,
        T015,
        T016,
        T017,
        T018,
        T019,

        FIRST = F001,
        LAST  = T019,
        COUNT
    };

    QString name(Type type);
    Type type(const QString& name);

    QString title(Type type);
    QString title(const QString& type);

    QString explanation(Type type);
    QString explanation(const QString& type);

    void init();
}

}

#endif
