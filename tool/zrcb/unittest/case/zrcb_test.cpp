#include <gtest/gtest.h>
#include <filesystem>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/handle.h>

namespace {

std::filesystem::path GetInputDirectoryPath() {
    return std::filesystem::path{ __FILEW__ }.parent_path().parent_path() / L"input";
}

std::filesystem::path GetOutputDirectoryPath() {
    return std::filesystem::path{ __FILEW__ }.parent_path().parent_path() / L"output";
}

void RunZrcb(const std::wstring& arguments) {

    auto exe_directory_path = zaf::Application::Instance().GetExeDirectoryPath();
    auto zrcb_path = exe_directory_path / "zrcb.exe";

    std::wstring command_line = zrcb_path.wstring() + L' ' + arguments;

    auto working_directory_path = GetOutputDirectoryPath();

    auto stdout_file_path = GetOutputDirectoryPath() / L"stdout.txt";

    SECURITY_ATTRIBUTES security_attributes{};
    security_attributes.bInheritHandle = TRUE;

    zaf::Handle stdout_handle{ CreateFile(
        stdout_file_path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        &security_attributes,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr) };

    if (stdout_handle.GetValue() == INVALID_HANDLE_VALUE) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    STARTUPINFO startup_info{};
    startup_info.cb = sizeof(startup_info);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdOutput = stdout_handle.GetValue();
    startup_info.hStdError = stdout_handle.GetValue();

    PROCESS_INFORMATION process_information{};

    BOOL is_succeeded = CreateProcess(
        nullptr,
        const_cast<wchar_t*>(command_line.c_str()),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        working_directory_path.c_str(),
        &startup_info,
        &process_information);

    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    CloseHandle(process_information.hThread);

    zaf::Handle process_handle{ process_information.hProcess };
    WaitForSingleObject(process_handle.GetValue(), INFINITE);

    DWORD exit_code{};
    is_succeeded = GetExitCodeProcess(process_handle.GetValue(), &exit_code);
    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    if (exit_code != 0) {
        ZAF_THROW_SYSTEM_ERROR(ERROR_INVALID_FUNCTION);
    }
}

}


TEST(TestZrcb, FullBuild) {

    RunZrcb(GetInputDirectoryPath().wstring());
}