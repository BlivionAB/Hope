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

template<typename TBaselineTestProject>
class TestRunner
{

public:

    explicit
    TestRunner(TBaselineTestProject& project);

    void
    execute(const TestSelection& testSelection);

private:

    TBaselineTestProject&
    _project;
};

#include "TestRunnerImpl.h"

#endif //ELET_TESTRUNNER_H
