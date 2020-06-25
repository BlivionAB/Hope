#ifndef ELET_TESTRUNNERMOCK_H
#define ELET_TESTRUNNERMOCK_H

#include <gmock/gmock.h>
#include "Foundation/TestRunner.h"

template <typename TBaselineProject>
class TestRunnerMock : public TestRunner<TBaselineProject>
{
public:
    using TestRunner<TBaselineProject>::TestRunner;

    MOCK_METHOD(void, execute, (const TestSelection& testSelection));
};


#endif //ELET_TESTRUNNERMOCK_H
