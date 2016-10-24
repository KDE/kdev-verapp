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

#include "rules.h"

#include <QHash>

namespace verapp
{

namespace rules
{
    static QString names[Type::COUNT];
    static QString titles[Type::COUNT];
    static QString explanations[Type::COUNT];

    static QHash<QString, Type> types;

    void initDb(QString* titles, QString* explanations);

    void init()
    {
        names[F001] = QStringLiteral("F001");
        names[F002] = QStringLiteral("F002");

        names[L001] = QStringLiteral("L001");
        names[L002] = QStringLiteral("L002");
        names[L003] = QStringLiteral("L003");
        names[L004] = QStringLiteral("L004");
        names[L005] = QStringLiteral("L005");
        names[L006] = QStringLiteral("L006");

        names[T001] = QStringLiteral("T001");
        names[T002] = QStringLiteral("T002");
        names[T003] = QStringLiteral("T003");
        names[T004] = QStringLiteral("T004");
        names[T005] = QStringLiteral("T005");
        names[T006] = QStringLiteral("T006");
        names[T007] = QStringLiteral("T007");
        names[T008] = QStringLiteral("T008");
        names[T009] = QStringLiteral("T009");
        names[T010] = QStringLiteral("T010");
        names[T011] = QStringLiteral("T011");
        names[T012] = QStringLiteral("T012");
        names[T013] = QStringLiteral("T013");
        names[T014] = QStringLiteral("T014");
        names[T015] = QStringLiteral("T015");
        names[T016] = QStringLiteral("T016");
        names[T017] = QStringLiteral("T017");
        names[T018] = QStringLiteral("T018");
        names[T019] = QStringLiteral("T019");

        for (int intType = FIRST; intType <= LAST; ++intType) {
            Type ruleType = static_cast<Type>(intType);

            types[names[ruleType]] = ruleType;
        }

        initDb(titles, explanations);
    }

    Type type(const QString& name)
    {
        return types[name];
    }

    QString name(Type type)
    {
        return names[type];
    }

    QString title(Type type)
    {
        return titles[type];
    }

    QString title(const QString& name)
    {
        return titles[type(name)];
    }

    QString explanation(Type type)
    {
        return explanation(name(type));
    }

    QString explanation(const QString& name)
    {
        return explanations[type(name)];
    }
}

}
