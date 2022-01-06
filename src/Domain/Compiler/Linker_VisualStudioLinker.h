#ifndef ELET_LINKER_VISUALSTUDIOLINKER_H
#define ELET_LINKER_VISUALSTUDIOLINKER_H


#include <filesystem>
#include <unordered_map>
#include <Foundation/Utf8String.h>

namespace elet::domain::compiler::linker::vs
{
    using namespace elet::foundation;
    namespace fs = std::filesystem;


    enum class VisualStudioVersion
    {
        None,
        _2022 = 2022,
    };


    enum class VisualStudioEdition
    {
        None,
        Enterprise = 3,
        Professional = 2,
        Community = 1,
    };


    struct VsDirectoryNotFoundError : std::exception
    {

    };


    enum class MsvcVersionLevel : uint8_t
    {
        Major,
        Minor,
        Build,
    };

    struct MsvcVersion
    {
        uint8_t
        major = 0;

        uint8_t
        minor = 0;

        uint16_t
        build = 0;

        MsvcVersion() = default;

        MsvcVersion(std::string version)
        {
            MsvcVersionLevel versionLevel = MsvcVersionLevel::Major;
            std::stack<unsigned int> versionInts;
            for (const char &c : version)
            {
                if (c == '.')
                {
                    assignVersionToVersionLevel(versionInts, versionLevel);
                    versionInts = std::stack<unsigned int>();
                    continue;
                }
                int digit = c - 48;
                if (digit < 0 && digit > 10)
                {
                    throw std::runtime_error("Could not get msvc version.");
                }
                versionInts.push(digit);
            }
            assignVersionToVersionLevel(versionInts, versionLevel);
        }


        void
        assignVersionToVersionLevel(std::stack<unsigned int>& versionInts, MsvcVersionLevel& versionLevel)
        {
            int value = 0;
            int exponent = 0;
            while (!versionInts.empty())
            {
                unsigned int digit = versionInts.top();
                value += digit * static_cast<unsigned int>(pow(10, exponent));
                exponent++;
                versionInts.pop();
            }
            switch (versionLevel)
            {
                case MsvcVersionLevel::Major:
                    major = value;
                    versionLevel = MsvcVersionLevel::Minor;
                    break;
                case MsvcVersionLevel::Minor:
                    minor = value;
                    versionLevel = MsvcVersionLevel::Build;
                    break;
                case MsvcVersionLevel::Build:
                    build = value;
                    break;
            }
        }


        bool
        operator > (const MsvcVersion& otherVersion)
        {
            if (major > otherVersion.major)
            {
                return true;
            }
            else if (major < otherVersion.major)
            {
                return false;
            }
            else if (minor > otherVersion.minor)
            {
                return true;
            }
            else if (minor < otherVersion.minor)
            {
                return false;
            }
            else if (build > otherVersion.build)
            {
                return true;
            }
            else if (build < otherVersion.build)
            {
                return false;
            }
            return true;
        }

        std::string
        toString()
        {
            return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build);
        }
    };


    class VisualStudioLinker
    {

    public:

        VisualStudioLinker();

        void
        link(const fs::path& objectPath, const fs::path& executablePath);

    private:

        fs::path
        _vsDirectory;

        fs::path
        getLinkerPath() const;

        fs::path
        getLibPath() const;

        static
        VisualStudioVersion
        findVersionFromVsDirectory(const fs::path& visualStudioDirectory);

        static
        VisualStudioEdition
        findEditionFromVersionDirectory(const fs::path& versionDirectory);

        void
        setVsDirecotry();

        MsvcVersion
        findMsvcVersionFromMsvcDirectory(const fs::path& msvcDirectory);

        static
        MsvcVersion
        toMsvcVersion(std::string versionString);

        std::string
        getEscapedStringPath(fs::path path) const;
    };
}


#endif //ELET_LINKER_VISUALSTUDIOLINKER_H
