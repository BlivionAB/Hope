#ifndef ELET_TESTRUNNERMOCK_H
#define ELET_TESTRUNNERMOCK_H

#include <gmock/gmock.h>
#include "Foundation/TestRunner.h"

template <typename TBaselineProject, typename TTestSelection>
class TestRunnerMock : public TestRunner<TBaselineProject, TTestSelection>
{
public:
    using TestRunner<TBaselineProject, TTestSelection>::TestRunner;

    MOCK_METHOD(void, execute, (const TestSelection& testSelection));
};


#endif //ELET_TESTRUNNERMOCK_H
