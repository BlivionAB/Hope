//
// Created by Tien Heng Ho on 2020-06-24.
//

#ifndef ELET_PATHMOCK_H
#define ELET_PATHMOCK_H

#include <gmock/gmock.h>
#include "Foundation/Path.h"

class PathMock: public Path
{
public:
    MOCK_METHOD(Path, cwd, ());
    MOCK_METHOD(bool, exists, (const Path& path));
    MOCK_METHOD(Path, resolve, (const Path& base, const Path& path));
};


#endif //ELET_PATHMOCK_H
