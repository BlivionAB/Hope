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

template<
    typename TBaselineTestProject,
    typename TPath>
class TestRunner
{

public:

    explicit
    TestRunner(TBaselineTestProject& project, TPath& path);

    void
    execute(const TestSelection& testSelection);

private:

    TBaselineTestProject&
    _project;

    Path&
    _path;
};

#include "TestRunnerImpl.h"

#endif //ELET_TESTRUNNER_H
