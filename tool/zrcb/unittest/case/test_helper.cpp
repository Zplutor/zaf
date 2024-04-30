#include "test_helper.h"
#include <fstream>
#include <string>
#include <zaf/base/error/win32_error.h>
#include <zaf/base/handle.h>
#include <zaf/base/string/replace.h>
#include <zaf/base/string/split.h>

namespace {

std::filesystem::path GetTestDataDirectoryPath() {
    return std::filesystem::path{ __FILEW__ }.parent_path().parent_path() / L"test_data";
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


//TODO: The code here is copied from zrcb, it should be reused.
std::vector<std::wstring> ReadLinesFromOutputFile(const std::filesystem::path& path) {

    std::ifstream input_stream(path, std::ios::in | std::ios::binary);
    if (!input_stream) {
        return {};
    }

    input_stream.seekg(0, std::ios::end);
    auto file_length = input_stream.tellg();

    if ((file_length < 2) ||
        (file_length % 2 != 0)) {
        return {};
    }

    //Subtract 2 to exclude the BOM.
    std::size_t content_length = static_cast<std::size_t>(file_length) - 2;

    std::wstring content;
    content.resize(content_length / 2);

    input_stream.seekg(2, std::ios::beg);
    input_stream.read(reinterpret_cast<char*>(&content[0]), content_length);

    if (input_stream.gcount() != content_length) {
        return {};
    }

    zaf::Replace(content, L"\r\n", L"\n");
    return zaf::Split(content, L'\n');
}

}


std::filesystem::path GetTemplateDirectoryPath() {
    return GetTestDataDirectoryPath() / "template";
}


std::filesystem::path GetWorkingDirectoryPath() {
    return GetTestDataDirectoryPath() / L"working";
}


std::filesystem::path GetInputDirectoryPath() {
    return GetWorkingDirectoryPath() / "input";
}


void RunZrcb(const std::wstring& arguments) {

    auto exe_directory_path = zaf::Application::Instance().GetExeDirectoryPath();
    auto zrcb_path = exe_directory_path / "zrcb.exe";

    std::wstring command_line = zrcb_path.wstring() + L' ' + arguments;

    auto working_directory_path = GetWorkingDirectoryPath();

    auto stdout_file_path = GetWorkingDirectoryPath() / L"stdout.txt";

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

    if (!stdout_handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    STARTUPINFO startup_info{};
    startup_info.cb = sizeof(startup_info);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdOutput = *stdout_handle;
    startup_info.hStdError = *stdout_handle;

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
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    CloseHandle(process_information.hThread);

    zaf::Handle process_handle{ process_information.hProcess };
    WaitForSingleObject(*process_handle, INFINITE);

    DWORD exit_code{};
    is_succeeded = GetExitCodeProcess(*process_handle, &exit_code);
    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    if (exit_code != 0) {
        ZAF_THROW_WIN32_ERROR(ERROR_INVALID_FUNCTION);
    }
}


std::wstring FindDefaultOutputRCFile() {

    auto output_path = GetWorkingDirectoryPath();
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

    auto output_file_path = GetWorkingDirectoryPath() / file_name;
    auto lines = ReadLinesFromOutputFile(output_file_path);

    std::vector<std::pair<std::wstring, std::wstring>> read_items;
    for (const auto& each_line : lines) {

        //Ignore empty lines.
        if (each_line.empty()) {
            continue;
        }

        //Ignore comment lines.
        if (each_line.find(L"//") == 0) {
            continue;
        }

        auto pair = ParseResourceItem(each_line);
        if (pair.first.empty()) {
            return false;
        }

        read_items.push_back(pair);
    }

    return read_items == expected_items;
}