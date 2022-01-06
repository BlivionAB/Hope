#include "Execute.h"

namespace elet::foundation
{
    int executeCommand(
        std::string command, //Command Line
        std::string cwd,  //set to '.' for current directory
        std::string& _stdout, //Return List of StdOut
        std::string& _stderr, //Return List of StdErr
        uint32_t& returnCode) //Return Exit Code
    {
        int Success;
        SECURITY_ATTRIBUTES  securityAttributes;
        HANDLE stdout_rd = INVALID_HANDLE_VALUE;
        HANDLE stdout_wr = INVALID_HANDLE_VALUE;
        HANDLE stderr_rd = INVALID_HANDLE_VALUE;
        HANDLE stderr_wr = INVALID_HANDLE_VALUE;
        PROCESS_INFORMATION processInfo;
        STARTUPINFO startupInfo;
        std::thread stdout_thread;
        std::thread stderr_thread;

        securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
        securityAttributes.bInheritHandle = TRUE;
        securityAttributes.lpSecurityDescriptor = nullptr;

        if (!CreatePipe(&stdout_rd, &stdout_wr, &securityAttributes, 0) ||
        !SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0))
        {
            return -1;
        }

        if (!CreatePipe(&stderr_rd, &stderr_wr, &securityAttributes, 0) ||
        !SetHandleInformation(stderr_rd, HANDLE_FLAG_INHERIT, 0))
        {
            if (stdout_rd != INVALID_HANDLE_VALUE) CloseHandle(stdout_rd);
            if (stdout_wr != INVALID_HANDLE_VALUE) CloseHandle(stdout_wr);
            return -2;
        }

        ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&startupInfo, sizeof(STARTUPINFO));

        startupInfo.cb = sizeof(STARTUPINFO);
        startupInfo.hStdInput = 0;
        startupInfo.hStdOutput = stdout_wr;
        startupInfo.hStdError = stderr_wr;

        if(stdout_rd || stderr_rd)
        {
            startupInfo.dwFlags |= STARTF_USESTDHANDLES;
        }

        // Make a copy because CreateProcess needs to modify string buffer
        char commandString[4096];
//        assert(command.size() <= MAX_PATH && "Command cannot exceed MAX_PATH.");
        std::strncpy(commandString, command.c_str(), 4096);
        commandString[4096 - 1] = 0;

        Success = CreateProcess(
            nullptr,
            commandString,
            nullptr,
            nullptr,
            TRUE,
            CREATE_NO_WINDOW,
            nullptr,
            cwd.c_str(),
            &startupInfo,
            &processInfo
        );
        CloseHandle(stdout_wr);
        CloseHandle(stderr_wr);

        if(!Success)
        {
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
            CloseHandle(stdout_rd);
            CloseHandle(stderr_rd);
            return -4;
        }
        else
        {
            CloseHandle(processInfo.hThread);
        }

        if (stdout_rd)
        {
            stdout_thread = std::thread([&]() {
                DWORD n;
                const size_t bufsize = 1000;
                char buffer [bufsize];
                for(;;) {
                    n = 0;
                    int Success = ReadFile(
                        stdout_rd,
                        buffer,
                        (DWORD)bufsize,
                        &n,
                        nullptr
                        );
                    if(!Success || n == 0)
                        break;
                    std::string s(buffer, n);
                    _stdout += s;
                }
            });
        }

        if (stderr_rd)
        {
            stderr_thread = std::thread([&]() {
                DWORD n;
                const size_t bufsize = 1000;
                char buffer [bufsize];
                for(;;) {
                    n = 0;
                    int Success = ReadFile(
                        stderr_rd,
                        buffer,
                        (DWORD)bufsize,
                        &n,
                        nullptr
                        );
                    if(!Success || n == 0)
                        break;
                    std::string s(buffer, n);
                    _stdout += s;
                }
            });
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);
        if (!GetExitCodeProcess(processInfo.hProcess, (DWORD*) &returnCode))
        {
            returnCode = -1;
        }

        CloseHandle(processInfo.hProcess);

        if (stdout_thread.joinable())
        {
            stdout_thread.join();
        }

        if (stderr_thread.joinable())
        {
            stderr_thread.join();
        }

        CloseHandle(stdout_rd);
        CloseHandle(stderr_rd);

        return 0;
    }
}