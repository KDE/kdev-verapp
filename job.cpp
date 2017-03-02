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

#include "job.h"

#include "debug.h"
#include "rules.h"
#include "utils.h"

#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <shell/problem.h>

#include <QApplication>
#include <QElapsedTimer>
#include <QRegularExpression>

namespace verapp
{

Job::Job(const Parameters &params)
    : OutputExecuteJob(nullptr)
    , m_timer(new QElapsedTimer)
{
    setJobName(i18n("Vera++ Analysis (%1)", prettyPathName(params.checkPath)));

    setCapabilities(KJob::Killable);
    setStandardToolView(KDevelop::IOutputView::TestView);
    setBehaviours(KDevelop::IOutputView::AutoScroll);

    setProperties(OutputExecuteJob::JobProperty::DisplayStdout);
    setProperties(OutputExecuteJob::JobProperty::DisplayStderr);
    setProperties(OutputExecuteJob::JobProperty::PostProcessOutput);

#if defined(_WIN32) || defined(_WIN64)
    *this << "cmd";
#else
    *this << "sh";
#endif

    *this << params.buildRunScript();

    qCDebug(KDEV_VERAPP) << "checking path" << params.checkPath;
}

Job::~Job()
{
}

void Job::postProcessStdout(const QStringList& lines)
{
    static const auto errorRegex    = QRegularExpression("(.+):(\\d+):\\s*([A-Z]\\d{3}):\\s*(.+)$");
    static const auto fileNameRegex = QRegularExpression("Checking ([^:]*)\\.{3}");
    static const auto percentRegex  = QRegularExpression("(\\d+)% done");

    QVector<KDevelop::IProblem::Ptr> problems;
    QRegularExpressionMatch match;

    for (const QString & line : lines) {
        match = errorRegex.match(line);
        if (match.hasMatch()) {
            KDevelop::IProblem::Ptr problem(new KDevelop::DetectedProblem);

            problem->setSource(KDevelop::IProblem::Plugin);
            problem->setSeverity(KDevelop::IProblem::Warning);
            problem->setDescription(match.captured(3) + ": " + match.captured(4));
            problem->setExplanation(rules::explanation(match.captured(3)));

            KDevelop::DocumentRange range;
            range.document = KDevelop::IndexedString(match.captured(1));
            range.setBothLines(match.captured(2).toInt() - 1);
            range.setBothColumns(0);
            problem->setFinalLocation(range);
            problem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);

            problems.append(problem);
            continue;
        }

        match = fileNameRegex.match(line);
        if (match.hasMatch()) {
            emit infoMessage(this, match.captured(1));
            continue;
        }

        match = percentRegex.match(line);
        if (match.hasMatch()) {
            setPercent(match.captured(1).toULong());
            continue;
        }
    }

    if (problems.size()) {
        emit problemsDetected(problems);
    }

    m_standardOutput << lines;

    if (status() == KDevelop::OutputExecuteJob::JobStatus::JobRunning) {
        OutputExecuteJob::postProcessStdout(lines);
    }
}

void Job::postProcessStderr(const QStringList& lines)
{
    m_stderrOutput << lines;

    if (status() == KDevelop::OutputExecuteJob::JobStatus::JobRunning) {
        OutputExecuteJob::postProcessStderr(lines);
    }
}

void Job::start()
{
    m_standardOutput.clear();
    m_stderrOutput.clear();

    qCDebug(KDEV_VERAPP) << "executing:" << commandLine().join(' ');

    m_timer->restart();
    OutputExecuteJob::start();
}

void Job::childProcessError(QProcess::ProcessError e)
{
    QString message;

    switch (e) {
    case QProcess::FailedToStart:
        message = i18n("Failed to start Vera++ from \"%1\".", commandLine()[0]);
        break;

    case QProcess::Crashed:
        if (status() != KDevelop::OutputExecuteJob::JobStatus::JobCanceled) {
            message = i18n("Vera++ crashed.");
        }
        break;

    case QProcess::Timedout:
        message = i18n("Vera++ process timed out.");
        break;

    case QProcess::WriteError:
        message = i18n("Write to Vera++ process failed.");
        break;

    case QProcess::ReadError:
        message = i18n("Read from Vera++ process failed.");
        break;

    case QProcess::UnknownError:
        // current vera++ errors will be displayed in the output view
        // don't notify the user
        break;
    }

    if (!message.isEmpty()) {
        KMessageBox::error(qApp->activeWindow(), message, i18n("Vera++ Error"));
    }

    KDevelop::OutputExecuteJob::childProcessError(e);
}

void Job::childProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(KDEV_VERAPP) << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    postProcessStdout({QString("Elapsed time: %1 s.").arg(m_timer->elapsed()/1000.0)});

    if (exitCode != 0) {
        qCDebug(KDEV_VERAPP) << "vera++ failed";
        qCDebug(KDEV_VERAPP) << "stdout output: ";
        qCDebug(KDEV_VERAPP) << m_standardOutput.join('\n');
        qCDebug(KDEV_VERAPP) << "stderr output: ";
        qCDebug(KDEV_VERAPP) << m_stderrOutput.join('\n');
    }

    KDevelop::OutputExecuteJob::childProcessExited(exitCode, exitStatus);
}

}
