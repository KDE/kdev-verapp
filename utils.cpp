/* This file is part of KDevelop

   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "utils.h"

#include <interfaces/icore.h>
#include <interfaces/iprojectcontroller.h>

#include <QMimeDatabase>

namespace verapp
{

QString prettyPathName(const QString& path)
{
    return KDevelop::ICore::self()->projectController()->prettyFileName(
        QUrl::fromLocalFile(path),
        KDevelop::IProjectController::FormatPlain);
}

bool isSupportedMimeType(const QMimeType& mimeType)
{
    const QString mimeName = mimeType.name();
    return (mimeName == QLatin1String("text/x-c++src") ||
            mimeName == QLatin1String("text/x-c++hdr") ||
            mimeName == QLatin1String("text/x-chdr")   ||
            mimeName == QLatin1String("text/x-csrc"));
}

bool isSupportedFile(const QString& fileName)
{
    return isSupportedMimeType(QMimeDatabase().mimeTypeForFile(fileName));
}

bool isSupportedUrl(const QUrl& url)
{
    return isSupportedMimeType(QMimeDatabase().mimeTypeForUrl(url));
}


}
