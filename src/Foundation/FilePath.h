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

class FilePath
{
public:

    static
    Utf8String
    extension(const FilePath& path);

    static
    FilePath
    folderOf(const FilePath& file);

    static
    const char*
    filename(const FilePath& file);


    static
    Utf8String
    stem(const FilePath& file);

    static
    FilePath
    cwd();

    static
    List<FilePath>
    find(const FilePath& path);

    static
    FilePath
    absolutePathOf(const FilePath& path, const FilePath& base = cwd());

    static
    FilePath
    resolve(const FilePath& base, const FilePath& path);

    static
    FilePath
    relativeTo(const FilePath& base, const FilePath& target);

    static
    bool
    isDescendentOf(const FilePath& parent, const FilePath& child);

    static
    bool
    exists(const FilePath& path);

    static
    FilePath
    getTemporaryFile();

    static
    FilePath
    getCurrentWorkingDirectory();

    static
    FilePath
    getTemporaryDirectory();

    FilePath();

    FilePath(const FilePath& other);

    FilePath(const char*);

    FilePath(Utf8String);

    ~FilePath();

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
    add(const FilePath& path);

    template<typename ...TPath>
    void
    add(const FilePath& path, TPath... other);

    bool
    isFile() const;

    Utf8String
    toString() const;

    unsigned int
    segmentLength() const;

    FilePath
    operator / (const char* path) const;

    FilePath
    operator / (const Utf8String& path) const;

    FilePath&
    operator = (const FilePath& other);

    FilePath&
    operator = (const FilePath&& other) noexcept;

    bool
    operator == (const FilePath& path) const;

    bool
    operator != (const FilePath& path) const;

    const char*
    operator [] (unsigned int index) const;

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
    segmentPathIntoWildcardPathSegments(const FilePath& path);

    static
    List<FilePath>
    findPathByPathSegmentList(WildcardPathSegmentStack pathSegmentList, const FilePath& currenPath);
};


struct TargetPathIsNotDescendentOfBasePathError
{

};


struct PathSegment
{
    FilePath path;

    PathSegment(FilePath path): path(path) { }
};


struct WildcardPathSegment
{
    FilePath path;

    WildcardPathSegment(FilePath path): path(path) { }
};


struct NestedWildcardPathSegment
{
    FilePath path;

    NestedWildcardPathSegment(FilePath path): path(path) { }
};


using OneOfPathSegments = std::variant<
    PathSegment,
    NestedWildcardPathSegment,
    WildcardPathSegment>;


template<typename ... TChar>
void
FilePath::add(const char* path, TChar ... other)
{
    add(path);
    add(other...);
}

#endif //FLASHPOINT_PATH_H
