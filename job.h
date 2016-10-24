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

#ifndef VERAPP_JOB_H
#define VERAPP_JOB_H


#include "parameters.h"

#include <interfaces/iproblem.h>
#include <outputview/outputexecutejob.h>

class QElapsedTimer;

namespace verapp
{

class Job : public KDevelop::OutputExecuteJob
{
    Q_OBJECT

public:
    explicit Job(const Parameters& params);
    ~Job() override;

    void start() override;

Q_SIGNALS:
    void problemsDetected(const QVector<KDevelop::IProblem::Ptr>& problems);

protected slots:
    void postProcessStdout(const QStringList& lines) override;
    void postProcessStderr(const QStringList& lines) override;

    void childProcessExited(int exitCode, QProcess::ExitStatus exitStatus) override;
    void childProcessError(QProcess::ProcessError processError) override;

protected:
    QScopedPointer<QElapsedTimer> m_timer;

    QStringList m_standardOutput;
    QStringList m_stderrOutput;
};

}
#endif
