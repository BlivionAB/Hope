#include "Linker_VisualStudioLinker.h"
#include <filesystem>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <Foundation/StringBuilder.h>
#include <Foundation/Execute.h>

namespace elet::domain::compiler::linker::vs
{
    namespace fs = std::filesystem;


    static std::unordered_map<VisualStudioEdition, std::string> VisualStudioEditionToStringMap =
    {
        { VisualStudioEdition::Community, "Community" },
        { VisualStudioEdition::Professional, "Professional" },
        { VisualStudioEdition::Enterprise, "Enterprise" },
    };


    static std::unordered_map<VisualStudioVersion, std::string> VisualStudioVersionToStringMap =
    {
        { VisualStudioVersion::_2022, "2022" }
    };


    VisualStudioLinker::VisualStudioLinker()
    {
        setVsDirecotry();

    }


    void
    VisualStudioLinker::setVsDirecotry()
    {
        bool foundVsDirectory = false;
        fs::path x64Directory("C:\\Program Files\\Microsoft Visual Studio\\");
        if (fs::exists(x64Directory))
        {
            _vsDirectory = x64Directory;
            foundVsDirectory = true;
        }
        fs::path x86Directory("C:\\Program Files (x86)\\Microsoft Visual Studio\\");
        if (!foundVsDirectory && fs::exists(x86Directory))
        {
            _vsDirectory = x86Directory;
        }
        VisualStudioVersion version = findVersionFromVsDirectory(_vsDirectory);
        if (version == VisualStudioVersion::None)
        {
            throw VsDirectoryNotFoundError();
        }
        _vsDirectory.append(VisualStudioVersionToStringMap[version]);
        VisualStudioEdition edition = findEditionFromVersionDirectory(_vsDirectory);
        if (edition == VisualStudioEdition::None)
        {
            throw VsDirectoryNotFoundError();
        }
        _vsDirectory.append(VisualStudioEditionToStringMap[edition]);
        _vsDirectory.append("VC\\Tools\\MSVC");
        if (!fs::exists(_vsDirectory))
        {
            throw VsDirectoryNotFoundError();
        }
        _vsDirectory.append(findMsvcVersionFromMsvcDirectory(_vsDirectory).toString());
    }





    VisualStudioVersion
    VisualStudioLinker::findVersionFromVsDirectory(const fs::path& visualStudioDirectory)
    {
        VisualStudioVersion version;
        VisualStudioVersion currentVersion = VisualStudioVersion::None;
        for (const fs::directory_entry& directory : fs::directory_iterator(visualStudioDirectory))
        {
            bool hasFoundVersion = false;
            fs::path versionDirectory = directory.path().filename();
            if (versionDirectory == "2022")
            {
                version = VisualStudioVersion::_2022;
                hasFoundVersion = true;
            }
            if (hasFoundVersion && version > currentVersion)
            {
                currentVersion = version;
            }
        }
        return currentVersion;
    }


    VisualStudioEdition
    VisualStudioLinker::findEditionFromVersionDirectory(const fs::path& versionDirectory)
    {
        VisualStudioEdition edition;
        VisualStudioEdition currentEdition = VisualStudioEdition::None;
        for (const fs::directory_entry& directory : fs::directory_iterator(versionDirectory))
        {
            bool hasFoundEdition = false;
            fs::path editionDirectory = directory.path().filename();
            if (editionDirectory == "Community")
            {
                edition = VisualStudioEdition::Community;
                hasFoundEdition = true;
            }
            else if (editionDirectory == "Professional")
            {
                edition = VisualStudioEdition::Professional;
                hasFoundEdition = true;
            }
            else if (editionDirectory == "Enterprise")
            {
                edition = VisualStudioEdition::Enterprise;
                hasFoundEdition = true;
            }
            if (hasFoundEdition && edition > currentEdition)
            {
                currentEdition = edition;
            }
        }
        return currentEdition;
    }


    MsvcVersion
    VisualStudioLinker::findMsvcVersionFromMsvcDirectory(const fs::path& msvcDirectory)
    {
        bool hasFoundVersion = false;
        MsvcVersion currentVersion;
        for (const fs::directory_entry& directory : fs::directory_iterator(msvcDirectory))
        {
            MsvcVersion version = toMsvcVersion(directory.path().filename().string());
            if (version > currentVersion)
            {
                currentVersion = version;
                hasFoundVersion = true;
            }
        }
        if (!hasFoundVersion)
        {
            throw VsDirectoryNotFoundError();
        }
        return currentVersion;
    }


    MsvcVersion
    VisualStudioLinker::toMsvcVersion(std::string versionString)
    {
        MsvcVersion version(versionString);
        return version;
    }


    void
    VisualStudioLinker::link(const fs::path& objectPath, const fs::path& executablePath)
    {
        StringBuilder command;
        command.append(getEscapedStringPath(getLinkerPath()));
        command.append(" ");
        command.append("/nologo ");
        command.append("/out:");
        command.append(executablePath.string());
        command.append(" ");
        command.append("/libpath:");
        command.append(getEscapedStringPath(getLibPath()));
        command.append(" ");
        command.append("/version:0.0 /machine:x64 /debug /incremental:no /subsystem:console ");
        command.append(objectPath.string());

        std::uint32_t returnCode;
        std::string _stdout;
        std::string _stderr;

        int rc = executeCommand(
            command.toString(),
            ".",
            _stdout,
            _stderr,
            returnCode
        );
        if (!_stdout.empty())
        {
            std::cout << _stdout << std::endl;
        }
        if (!_stderr.empty())
        {
            std::cerr << _stderr << std::endl;
        }
    }

    std::string
    VisualStudioLinker::getEscapedStringPath(fs::path path) const
    {
        std::string linkPathString = path.make_preferred().string();
        replaceAll(linkPathString, "\\", "\\\\");
        return std::string("\"") + linkPathString + std::string("\"");
    }


    fs::path
    VisualStudioLinker::getLinkerPath() const
    {
        return (_vsDirectory / "bin\\Hostx64\\x64\\link.exe");
    }


    fs::path
    VisualStudioLinker::getLibPath() const
    {
        return _vsDirectory / "lib\\x64";
    }
}