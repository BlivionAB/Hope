#ifndef FLASHPOINT_PATH_H
#define FLASHPOINT_PATH_H

#include <dirent.h>
#include <variant>
#include "Stack.h"
#include "List.h"
#include "Utf8String.h"

using namespace elet::foundation;

struct PathSegment;
struct WildcardPathSegment;
struct NestedWildcardPathSegment;

class Path
{
public:

    static
    Utf8String
    extension(const Path& path);

    static
    Path
    directory(const Path& path);

    static
    Path
    cwd();

    static
    List<Path>
    find(const Path& path);

    static
    Path
    absolutePathOf(const Path& path, const Path& base = cwd());

    static
    Path
    relative(const Path& base, const Path& path);

    static
    bool
    isDescendentOf(const Path& parent, const Path& child);

    static
    bool
    exists(const Path& path);

    static
    Path
    getTemporaryFile();

    static
    Path
    getCurrentWorkingDirectory();

    static
    Path
    getTemporaryDirectory();

    Path();

    Path(const Path& other);

    Path(const char*);

    Path(Utf8String);

    ~Path();

    void
    up();

    void
    up(int folders);

    bool
    isAbsolute() const;

    bool
    isRelative() const;

    bool
    startsWith(const char* string) const;

    void
    add(const char* path);

    template<typename ...TChar>
    void
    add(const char* path, TChar... other);

    void
    add(const Path& path);

    template<typename ...TPath>
    void
    add(const Path& path, TPath... other);

    bool
    isFile() const;

    Utf8String
    toString() const;

    Path
    operator / (const char* path) const;

    Path
    operator / (const Utf8String& path) const;

    Path&
    operator = (const Path& other);

    Path&
    operator = (const Path&& other) noexcept;

    bool
    operator == (const Path& path) const;

    bool
    operator != (const Path& path) const;

    operator const char*();

    Stack<const char*>::Iterator
    begin() const;

    Stack<const char*>::Iterator
    end() const;

    typedef std::variant<PathSegment, WildcardPathSegment, NestedWildcardPathSegment> OneOfWildcardPathSegment;

    typedef Stack<OneOfWildcardPathSegment> WildcardPathSegmentStack;

private:

    Stack<const char*>
    _segments;

#ifdef DEBUG
    std::string
    __debug;
#endif

    void
    distributeSegmentsFromPath(const char* path);

    void
    handlePathSegment(const char* segment);

    static
    WildcardPathSegmentStack
    segmentPathIntoWildcardPathSegments(const Path& path);

    static
    List<Path>
    findPathByPathSegmentList(WildcardPathSegmentStack pathSegmentList, const Path& currenPath);
};

struct PathSegment
{
    Path path;

    PathSegment(Path path): path(path) { }
};

struct WildcardPathSegment
{
    Path path;

    WildcardPathSegment(Path path): path(path) { }
};

struct NestedWildcardPathSegment
{
    Path path;

    NestedWildcardPathSegment(Path path): path(path) { }
};

using OneOfPathSegments = std::variant<
    PathSegment,
    NestedWildcardPathSegment,
    WildcardPathSegment>;

template<typename ... TChar>
void
Path::add(const char* path, TChar ... other)
{
    add(path);
    add(other...);
}

#endif //FLASHPOINT_PATH_H
