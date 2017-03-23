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

#include "test_verappjob.h"

#include "job.h"

#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <language/editor/documentrange.h>

#include <QtTest/QTest>

using namespace KDevelop;
using namespace verapp;

class JobTester : public Job
{
    Q_OBJECT

public:
    explicit JobTester(const Parameters& params) : Job(params)
    {
        connect(this, &JobTester::problemsDetected, this, &JobTester::problemsDetectedSlot);
    }

    using Job::postProcessStdout;

    QVector<KDevelop::IProblem::Ptr> problems()
    {
        return m_problems;
    }

private:
    void problemsDetectedSlot(const QVector<KDevelop::IProblem::Ptr>& problems)
    {
        m_problems += problems;
    }

    QVector<KDevelop::IProblem::Ptr> m_problems;
};

void TestVerappJob::initTestCase()
{
    AutoTestShell::init({"kdevverapp"});
    TestCore::initialize(Core::NoUi);
}

void TestVerappJob::cleanupTestCase()
{
    TestCore::shutdown();
}

void TestVerappJob::testJob()
{
    QStringList stdoutOutput1 = {
        "source1.cpp:35: L004: line is longer than 100 characters",
        "source1.cpp:37: T009: comma should not be preceded by whitespace",
        "source1.cpp:42: T011: closing curly bracket not in the same line or column",
    };

    QStringList stdoutOutput2 = {
        "source2.cpp:13: T012: negation operator used in its short form",
        "source2.cpp:30: T019: full block {} expected in the control structure"
    };

    Parameters jobParams;
    JobTester jobTester(jobParams);

    // test incremental parsing
    jobTester.postProcessStdout(stdoutOutput1);
    QCOMPARE(jobTester.problems().size(), 3);

    jobTester.postProcessStdout(stdoutOutput2);
    QCOMPARE(jobTester.problems().size(), 5);

    // test common values
    auto problems = jobTester.problems();
    foreach (auto problem, problems) {
        QCOMPARE(problem->source(), KDevelop::IProblem::Plugin);
        QCOMPARE(problem->severity(), KDevelop::IProblem::Warning);
    }

    // test description
    QCOMPARE(problems[0]->description(), QStringLiteral("L004: line is longer than 100 characters"));
    QCOMPARE(problems[1]->description(), QStringLiteral("T009: comma should not be preceded by whitespace"));
    QCOMPARE(problems[2]->description(), QStringLiteral("T011: closing curly bracket not in the same line or column"));
    QCOMPARE(problems[3]->description(), QStringLiteral("T012: negation operator used in its short form"));
    QCOMPARE(problems[4]->description(), QStringLiteral("T019: full block {} expected in the control structure"));

    // test problem location (file)
    QCOMPARE(problems[0]->finalLocation().document.str(), QStringLiteral("source1.cpp"));
    QCOMPARE(problems[1]->finalLocation().document.str(), QStringLiteral("source1.cpp"));
    QCOMPARE(problems[2]->finalLocation().document.str(), QStringLiteral("source1.cpp"));

    QCOMPARE(problems[3]->finalLocation().document.str(), QStringLiteral("source2.cpp"));
    QCOMPARE(problems[4]->finalLocation().document.str(), QStringLiteral("source2.cpp"));

    // test problem location (line)
    QCOMPARE(problems[0]->finalLocation().start().line(), 34);
    QCOMPARE(problems[1]->finalLocation().start().line(), 36);
    QCOMPARE(problems[2]->finalLocation().start().line(), 41);

    QCOMPARE(problems[3]->finalLocation().start().line(), 12);
    QCOMPARE(problems[4]->finalLocation().start().line(), 29);
}

QTEST_GUILESS_MAIN(TestVerappJob)

#include "test_verappjob.moc"
