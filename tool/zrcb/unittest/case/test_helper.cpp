#include "test_helper.h"
#include <fstream>
#include <string>
#include <zaf/base/handle.h>

namespace {

std::filesystem::path GetTestDataDirectoryPath() {
    return std::filesystem::path{ __FILEW__ }.parent_path().parent_path() / "test_data";
}


std::pair<std::wstring, std::wstring> ParseResourceItem(const std::wstring& line) {

    std::pair<std::wstring, std::wstring> result;

    auto first_delimiter_index = line.find(' ');
    if (first_delimiter_index == std::string::npos) {
        return {};
    }

    result.first = line.substr(0, first_delimiter_index);

    auto second_delimiter_idnex = line.find(' ', first_delimiter_index + 1);
    if (second_delimiter_idnex == std::string::npos) {
        return {};
    }

    auto type_name = line.substr(
        first_delimiter_index + 1, 
        second_delimiter_idnex - first_delimiter_index - 1);

    if (type_name != L"ZRC") {
        return {};
    }

    result.second = line.substr(second_delimiter_idnex + 1);
    return result;
}

}


std::filesystem::path GetTemplateDirectoryPath() {
    return GetTestDataDirectoryPath() / "template";
}


std::filesystem::path GetInputDirectoryPath() {
    return GetTestDataDirectoryPath() / "input";
}


std::filesystem::path GetOutputDirectoryPath() {
    return GetTestDataDirectoryPath() / "output";
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


std::wstring FindDefaultOutputRCFile() {

    auto output_path = GetOutputDirectoryPath();
    for (const auto& each_file : std::filesystem::directory_iterator(output_path)) {

        if (each_file.is_directory()) {
            continue;
        }

        auto file_name = each_file.path().filename();
        if (file_name.extension() != L".rc") {
            continue;
        }

        auto file_name_string = file_name.wstring();
        if (file_name_string.find(L"zrc-") != 0) {
            continue;
        }

        return file_name_string;
    }

    return {};
}


bool CheckOutputRC(
    const std::wstring& file_name,
    const std::vector<std::pair<std::wstring, std::wstring>>& expected_items) {

    auto output_file_path = GetOutputDirectoryPath() / file_name;
    std::wifstream file_stream(output_file_path, std::ios::in);
    if (!file_stream) {
        return false;
    }

    std::vector<std::pair<std::wstring, std::wstring>> read_items;

    std::wstring line;
    while (std::getline(file_stream, line)) {

        //Ignore empty lines.
        if (line.empty()) {
            continue;
        }

        //Ignore comment lines.
        if (line.find(L"//") == 0) {
            continue;
        }

        auto pair = ParseResourceItem(line);
        if (pair.first.empty()) {
            return false;
        }

        read_items.push_back(pair);
    }

    return read_items == expected_items;
}