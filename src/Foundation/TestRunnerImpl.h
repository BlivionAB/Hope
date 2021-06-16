#include "TestRunner.h"


template<typename TBaselineTestProject, typename TTestSelection>
TestRunner<TBaselineTestProject, TTestSelection>::TestRunner(TBaselineTestProject& project):
    _project(project)
{

}
