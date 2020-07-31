#include "Path.h"
#include "Character.h"
#include <glob.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <stdlib.h>
#include <fnmatch.h>
#include <variant>
#include <regex>
#include <string>

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#include <boost/filesystem.hpp>

#define GetCurrentDir getcwd
#endif

#ifdef _WIN32
#include <windows.h>
#endif

const char kPathSeparator =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif


Utf8String
Path::extension(const Path& path)
{
    boost::filesystem::path p(path.toString().toString());
    return Utf8String(boost::filesystem::extension(p).c_str());
}


Path
Path::cwd()
{
    boost::filesystem::path cwd = boost::filesystem::current_path().c_str();
    const char* cwdString = cwd.c_str();
    std::size_t size = std::strlen(cwdString);
    char* copyOfCwd = new char[size + 1];
    std::strcpy(copyOfCwd, cwdString);
    return copyOfCwd;
}


Path
Path::getTemporaryFile()
{
    Path tempDir = getTemporaryDirectory();
    char* tempDirString = const_cast<char*>(static_cast<const char*>(tempDir / "XXXXXX"));
    int result = mkstemp(tempDirString);
    if (result == -1)
    {
        if (errno == EEXIST)
        {
            throw std::logic_error("Could not create folder");
        }
        if (errno == EINVAL)
        {
            throw std::logic_error("The last six characters of template were not XXXXXX; now template is unchanged.");
        }
    }
    return Path(tempDirString);
}


Path
Path::getCurrentWorkingDirectory()
{
    char filename[FILENAME_MAX];
    GetCurrentDir(filename, FILENAME_MAX);
    return Path(filename);
}


Path
Path::getTemporaryDirectory()
{
#ifdef _WIN32
    const char* temporaryDirectory = GetTempPath();
#else
    char* tmpDir = getenv("TMPDIR");
    if (tmpDir)
    {
        return Path(tmpDir);
    }
    tmpDir = getenv("TMP");
    if (tmpDir)
    {
        return Path(tmpDir);
    }
    tmpDir = getenv("TEMP");
    if (tmpDir)
    {
        return Path(tmpDir);
    }
    tmpDir = getenv("TEMPDIR");

    if (tmpDir)
    {
        return Path(tmpDir);
    }
    return Path("/tmp");
#endif
}


bool
Path::exists(const Path& path)
{
    return boost::filesystem::exists(boost::filesystem::path(path.toString().toString()));
}


List<Path>
Path::find(const Path& path)
{
    WildcardPathSegmentStack segmentationList = segmentPathIntoWildcardPathSegments(path);
    Path currentPath;
    return findPathByPathSegmentList(segmentationList, currentPath);
}


List<Path>
Path::findPathByPathSegmentList(Path::WildcardPathSegmentStack pathSegmentList, const Path& path)
{
    List<Path> result;
    Path currentPath = path;
    static int option = FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD;
    while (!pathSegmentList.isEmpty())
    {
        OneOfWildcardPathSegment oneOfPathSegment = pathSegmentList.top();
        bool isNestedWildcard = false;
        if (std::holds_alternative<NestedWildcardPathSegment>(oneOfPathSegment))
        {
            NestedWildcardPathSegment pathSegment = std::get<NestedWildcardPathSegment>(oneOfPathSegment);
            const char* directoryPathString = currentPath.toString().toString();
            boost::filesystem::path directoryPath(directoryPathString);
            boost::filesystem::directory_iterator directoryIterator(directoryPath), end;
            while (directoryIterator != end)
            {
                Path pathSegmentPath = pathSegment.path;
                if (boost::filesystem::is_directory(*directoryIterator))
                {
                    result.add(findPathByPathSegmentList(pathSegmentList, currentPath / directoryIterator->path().filename().c_str()));
                }
                else if (boost::filesystem::is_regular_file(*directoryIterator) && pathSegmentList.size() == 1)
                {
                    result.add(currentPath / directoryIterator->path().filename().c_str());
                }
                ++directoryIterator;
            }
            isNestedWildcard = true;
        }
        pathSegmentList.pop();

        if (isNestedWildcard)
        {
            // When ending in "/**", we have added all the necessary files in the logic above.
            if (pathSegmentList.isEmpty())
            {
                break;
            }
            oneOfPathSegment = pathSegmentList.top();
            pathSegmentList.pop();
        }

        if (std::holds_alternative<PathSegment>(oneOfPathSegment))
        {
            currentPath.add(std::get<PathSegment>(oneOfPathSegment).path);
            const char* currenPathString = currentPath.toString().toString();
            boost::filesystem::path fileCandidate(currenPathString);
            if (boost::filesystem::is_regular_file(fileCandidate))
            {
                const char* filename = fileCandidate.filename().c_str();
                if (fnmatch(currenPathString, filename, option))
                {
                    result.add(currentPath);
                }
            }
            continue;
        }
        else if (std::holds_alternative<WildcardPathSegment>(oneOfPathSegment))
        {
            WildcardPathSegment pathSegment = std::get<WildcardPathSegment>(oneOfPathSegment);
            const char* directoryPathString = currentPath.toString().toString();
            boost::filesystem::path directoryPath(directoryPathString);
            boost::filesystem::directory_iterator directoryIterator(directoryPath), end;
            while (directoryIterator != end)
            {
                Path pathSegmentPath = pathSegment.path;
                if (boost::filesystem::is_directory(*directoryIterator) && pathSegmentPath.toString() == "*")
                {
                    result.add(findPathByPathSegmentList(pathSegmentList, currentPath / directoryIterator->path().filename().c_str()));
                }
                else if (boost::filesystem::is_regular_file(*directoryIterator))
                {
                    boost::filesystem::path filename = directoryIterator->path().filename();
                    if (fnmatch(pathSegmentPath.toString().toString(), filename.c_str(), option))
                    {
                        result.create(currentPath / filename.c_str());
                    }
                }
                ++directoryIterator;
            }
            continue;
        }
    }
    return result;
}


Path::WildcardPathSegmentStack
Path::segmentPathIntoWildcardPathSegments(const Path& path)
{
    WildcardPathSegmentStack stack;
    Path currentPathSegment;
    bool currentPathSegmentIsEmpty = true;
    for (const char* pathSegment : path)
    {
        if (pathSegment[0] == '*' && strlen(pathSegment) > 1)
        {
            if (!currentPathSegmentIsEmpty)
            {
                stack.create<PathSegment>(currentPathSegment);
            }
            std::size_t length = strlen(pathSegment);
            if (length == 2 && pathSegment[1] == '*')
            {
                stack.create<NestedWildcardPathSegment>(pathSegment);
            }
            else
            {
                stack.create<WildcardPathSegment>(pathSegment);
            }
            currentPathSegment = Path();
            currentPathSegmentIsEmpty = true;
        }
        else
        {
            currentPathSegment.add(pathSegment);
            currentPathSegmentIsEmpty = false;
        }
    }
    if (!currentPathSegmentIsEmpty)
    {
        stack.create<PathSegment>(currentPathSegment);
    }
    stack.reverse();
    return stack;
}


Path
Path::absolutePathOf(const Path& path, const Path& base)
{
    const char* pathString = path.toString().toString();
    const char* basePathString = base.toString().toString();
    boost::filesystem::path absolutePath = boost::filesystem::absolute(pathString, basePathString);
    const char* absolutePathString = absolutePath.c_str();
    std::size_t size = std::strlen(absolutePathString) + 1;
    char* copyOfAbsolutePathString = new char[size];
    std::strcpy(copyOfAbsolutePathString, absolutePathString);
    return copyOfAbsolutePathString;
}

Path::Path()
{

}


Path::Path(const Path& other)
{
    _segments = other._segments;

#ifdef DEBUG
    __debug = toString().toString();
#endif
}


Path::Path(const char* path)
{
    distributeSegmentsFromPath(path);
}


Path::Path(Utf8String path)
{
    distributeSegmentsFromPath(path.toString());
}


Path::~Path()
{

}


void
Path::up()
{
    if (_segments.isEmpty())
    {
        _segments.push("..");
        return;
    }
    _segments.pop();
}


void
Path::up(int folders)
{
    for (int i = 0; i < folders; i++)
    {
        up();
    }
}


bool
Path::startsWith(const char* string) const
{
    const char* targetString = string;
    if (_segments.size() == 0)
    {
        return false;
    }
    Utf8String thisPathUtf8String = toString();
    const char* thisPathString = thisPathUtf8String.asString();
    std::size_t sizeOfThisPathString = strlen(thisPathString);
    std::size_t sizeOfTargetString = strlen(string);
    return sizeOfThisPathString < sizeOfTargetString
        ? false
        : std::memcmp(thisPathString, targetString, sizeOfTargetString) == 0;
}


bool
Path::isAbsolute() const
{
    boost::filesystem::path currentPath(toString().toString());
    return currentPath.is_absolute();
}


bool
Path::isRelative() const
{
    boost::filesystem::path currentPath(toString().toString());
    return currentPath.is_relative();
}


void
Path::add(const Path& path)
{
    for (const char* segment : path._segments)
    {
        _segments.push(segment);
    }
#ifdef DEBUG
    __debug = toString().toString();
#endif
}


template<typename ... TPath>
void
Path::add(const Path& path, TPath ... other)
{
    add(path);
    add(other...);
}


void
Path::add(const char* path)
{
    distributeSegmentsFromPath(path);
}


Utf8String
Path::toString() const
{
    if (_segments.size() > 0 && strlen(_segments[0]) >= 1 &&_segments[0][0] == '/')
    {
        Utf8String pathString("/");
        std::size_t lastIndex = _segments.size() - 1;
        if (lastIndex == 0)
        {
            return pathString;
        }
        pathString += _segments[1];
        std::size_t index = 2;
        while (index <= lastIndex)
        {
            pathString += "/";
            pathString += _segments[index];
            index++;
        }
        return pathString;
    }
    return _segments.join("/");
}


void
Path::distributeSegmentsFromPath(const char* path)
{
    const char* currentSegment = path;
    std::size_t currentSegmentSize = 0;
    while (true)
    {
        switch (currentSegment[currentSegmentSize])
        {
#ifdef _WIN32
            case '\\':
#endif
            case '/':
            {
                if (currentSegment == path && currentSegmentSize == 0)
                {
                    char* segment = new char[2];
                    std::strncpy(segment, &currentSegment[0], 1);
                    segment[1] = '\0';
                    _segments.push(segment);
                    currentSegment = &currentSegment[1];
                    currentSegmentSize = 0;
                    continue;
                }
                char* segment = new char[currentSegmentSize + 1];
                std::strncpy(segment, currentSegment, currentSegmentSize);
                segment[currentSegmentSize] = '\0';
                handlePathSegment(segment);
                currentSegment = &currentSegment[currentSegmentSize + 1];
                currentSegmentSize = 0;
                break;
            }
            case '\0':
            {
                if (currentSegmentSize != 0)
                {
                    char* segment = new char[currentSegmentSize + 1];
                    std::strncpy(segment, currentSegment, currentSegmentSize);
                    segment[currentSegmentSize] = '\0';
                    handlePathSegment(segment);
                }

#ifdef DEBUG
                __debug = toString().toString();
#endif
                return;
            }
        }
        currentSegmentSize++;
    }

#ifdef DEBUG
    __debug = toString().toString();
#endif
}


bool
Path::isFile() const
{
    boost::filesystem::path p(toString().toString());
    return boost::filesystem::is_regular_file(p);
}


Path::operator const char* ()
{
    return toString().toString();
}


Stack<const char*>::Iterator
Path::begin() const
{
    return _segments.begin();
}


Stack<const char*>::Iterator
Path::end() const
{
    return _segments.end();
}


Path
Path::operator / (const char* path) const
{
    Path result(toString().toString());
    result.add(path);
    return result;
}


Path
Path::operator / (const Utf8String& path) const
{
    Path result(toString().toString());
    result.add(path.toString());
    return result;
}


Path&
Path::operator = (const Path& other)
{
    _segments = other._segments;

#ifdef DEBUG
    __debug = toString().toString();
#endif
    return *this;
}


Path&
Path::operator = (const Path&& other) noexcept
{
    _segments = other._segments;
    return *this;
}


void
Path::handlePathSegment(const char* segment)
{
    switch (segment[0])
    {
        case '\0':
            return;
        case '.':
            if (std::strcmp(segment, "..") == 0)
            {
                up();
            }
            break;

        default:
            _segments.push(segment);
            break;
    }
}


bool
Path::isDescendentOf(const Path& parent, const Path& child)
{
    return child.startsWith(parent.toString().asString());
}


Path
Path::resolve(const Path& base, const Path& path)
{
    const char* baseString = base.toString().toString();
    const char* targetString = path.toString().toString();
    boost::filesystem::path relativePath = boost::filesystem::path(baseString) / boost::filesystem::path(targetString);
    const char* relativePathString = relativePath.c_str();
    std::size_t size = strlen(relativePathString);
    char* copyOfRelativePathString = new char[size + 1];
    std::strcpy(copyOfRelativePathString, relativePathString);
    return Path(copyOfRelativePathString);
}


bool
Path::operator == (const Path& path) const
{
    return toString() == path.toString();
}


bool
Path::operator != (const Path& path) const
{
    return toString() != path.toString();
}


Path
Path::folderOf(const Path& file)
{
    boost::filesystem::path p(file.toString().asString());
    return p.parent_path().c_str();
}


Utf8String
Path::stem(const Path& file)
{
    boost::filesystem::path p(file.toString().asString());
    const char* stem = p.stem().c_str();
    const std::size_t size = strlen(stem);
    char* result = new char[size + 1];
    std::strncpy(result, stem, size);
    return result;
}


const char*
Path::filename(const Path& file)
{
    boost::filesystem::path p(file.toString().asString());
    std::string text = p.filename().string();
    return text.c_str();
}


Path
Path::relativeTo(const Path& base, const Path& target)
{
    Path result;
    unsigned int baseLength = base.segmentLength();
    unsigned int targetLength = target.segmentLength();
    if (targetLength < baseLength)
    {
        throw TargetPathIsNotDescendentOfBasePathError();
    }
    for (int i = 0; i < targetLength; i++)
    {
        if (i <= baseLength - 1)
        {
            if (std::strcmp(base[i], target[i]) != 0)
            {
                throw TargetPathIsNotDescendentOfBasePathError();
            }
            continue;
        }
        result.add(target[i]);
    }
    return result;
}


unsigned int
Path::segmentLength() const
{
    return _segments.size();
}


const char*
Path::operator[](unsigned int index) const
{
    return _segments[index];
}
