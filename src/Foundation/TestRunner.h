#ifndef ELET_TESTRUNNER_H
#define ELET_TESTRUNNER_H

#include "BaselineTestProject.h"

struct TestSelection
{
    const char*
    test;

    const char*
    folder;
};


template<typename TBaselineTestProject, typename TTestSelection>
class TestRunner
{

public:

    explicit
    TestRunner(TBaselineTestProject& project);

    void
    execute(const TTestSelection& testSelection);

private:

    TBaselineTestProject&
    _project;
};

#include "TestRunnerImpl.h"

#endif //ELET_TESTRUNNER_H
