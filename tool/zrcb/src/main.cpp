#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <zaf/base/string/replace.h>
#include <zaf/base/string/split.h>

class Options {
public:
    std::filesystem::path output_file_path;
    std::vector<std::wstring> included_extensions;
    bool use_incremental_build{};
    bool use_back_slash{};
    bool print_help{};
};

class InputItem {
public:
    std::filesystem::path path;
    std::wstring name;
};

class ResourceItem {
public:
    std::wstring name;
    std::filesystem::path path;
};

bool operator==(const ResourceItem& item1, const ResourceItem& item2) {
    return (item1.name == item2.name) && (item1.path == item2.path);
}

bool operator<(const ResourceItem& item1, const ResourceItem& item2) {

    if (item1.name < item2.name) {
        return true;
    }
    if (item1.name > item2.name) {
        return false;
    }
    return item1.path < item2.path;
}


std::filesystem::path g_working_directory;
Options g_options;
std::vector<InputItem> g_input_items;


#define EXIT throw std::exception{}


void PrintHelp() {

    std::wcout <<
LR"(Zaf rc builder

Usage: 
  zrcb.exe [options] input1 [input2 [input3 [...]]]

Options:
  /help, /h
  Display help.

  /out, /o
  Specify an output file path. If this option is absent, the output file would
  be created in the working directory.

  /extension, /e
  Specify an extension list to filter files in input directories. The option 
  value should follow the following format:
  .ext1.ext2.ext3.ext4

  /incremental, /i
  Use incremental build.

  /back-slash, /b
  Use back slash as path delimiter instead of forward slash.
)";

}


bool IsHelpOption(const std::wstring_view& argument) {
    return argument == L"/help" || argument == L"/h";
}

bool IsOuptutFileOption(const std::wstring_view& argument) {
    return argument == L"/out" || argument == L"/o";
}

bool IsExtensionOption(const std::wstring_view& argument) {
    return argument == L"/extension" || argument == L"/e";
}

bool IsIncrementalBuildOption(const std::wstring_view& argument) {
    return argument == L"/incremental" || argument == L"/i";
}

bool IsBackSlashOption(const std::wstring_view& argument) {
    return argument == L"/back-slash" || argument == L"/b";
}

bool IsOptionArgument(const std::wstring_view& argument) {
    return argument.front() == L'/';
}


std::vector<std::wstring> ParseExtensions(const std::wstring_view& argument) {

    if (argument.front() != L'.') {
        std::wcout << L"Invalid option value \"" << argument << L"\".";
        EXIT;
    }

    std::vector<std::wstring> result;

    std::size_t current_index = 0;
    while (current_index < argument.length()) {

        auto next_dot_index = argument.find('.', current_index + 1);

        auto extension = argument.substr(current_index, next_dot_index - current_index);
        result.push_back(std::wstring{ extension.data(), extension.length() });

        current_index = next_dot_index;
    }

    return result;
}


void ParseInputItem(const std::wstring_view& argument) {

    InputItem input_item;

    auto delimiter_index = argument.find(L'*');
    if (delimiter_index == std::wstring_view::npos) {
        input_item.path = argument;
    }
    else {
        input_item.path = argument.substr(0, delimiter_index);
        input_item.name = argument.substr(delimiter_index + 1);
    }

    if (input_item.path.empty()) {
        std::wcout << L"Invalid argument \"" << argument << L"\"." << std::endl;
        EXIT;
    }

    g_input_items.push_back(input_item);
}


void ParseArguments(int argc, wchar_t** argv) {

    if (argc < 2) {
        g_options.print_help = true;
        return;
    }

    int index = 1;
    while (index < argc) {

        const wchar_t* argument = argv[index];

        //Help
        if (IsHelpOption(argument)) {
            g_options.print_help = true;
            ++index;
        }
        //Output file path
        else if (IsOuptutFileOption(argument)) {

            int next_argument_index = index + 1;
            if (next_argument_index >= argc) {
                std::wcout << L"A file path is needed for option \"" << argument << L"\"." 
                    << std::endl;
                EXIT;
            }

            g_options.output_file_path = argv[next_argument_index];
            index = next_argument_index + 1;
        }
        //Extension list
        else if (IsExtensionOption(argument)) {

            int next_argument_index = index + 1;
            if (next_argument_index >= argc) {
                std::wcout << L"An extension list is needed for option \"" << argument << L"\"."
                    << std::endl;
                EXIT;
            }

            g_options.included_extensions = ParseExtensions(argv[next_argument_index]);
            index = next_argument_index + 1;
        }
        //Incremental build
        else if (IsIncrementalBuildOption(argument)) {

            g_options.use_incremental_build = true;
            ++index;
        }
        //Back slash
        else if (IsBackSlashOption(argument)) {

            g_options.use_back_slash = true;
            ++index;
        }
        //Unknown options
        else if (IsOptionArgument(argument)) {

            std::wcout << L"Unknown option \"" << argument << L"\"." << std::endl;
            EXIT;
        }
        //Input items
        else {

            ParseInputItem(argument);
            ++index;
        }
    }
}


void InitializeWorkingDirectory() {

    wchar_t buffer[MAX_PATH]{};
    DWORD result = GetCurrentDirectory(MAX_PATH, buffer);
    if (result == 0) {
        std::wcout << L"Get working directory failed with error code " << GetLastError << L'.'
            << std::endl;
        EXIT;
    }

    g_working_directory = buffer;
    std::wcout << L"Working directory: " << g_working_directory << std::endl;
}


void PrintParsedArguments() {

    std::wcout
        << L"Output file path: " << g_options.output_file_path << L'\n'
        << L"Use incremental build: " << g_options.use_incremental_build << L'\n'
        << L"Use back slash: " << g_options.use_back_slash << L'\n';

    std::wcout << L"Included extensions: " << L'\n';
    for (const auto& each_extension : g_options.included_extensions) {
        std::wcout << each_extension << '\n';
    }

    std::wcout << L"Input items: " << L'\n';
    for (const auto& each_item : g_input_items) {

        std::wcout << each_item.path;
        if (!each_item.name.empty()) {
            std::wcout << L" -> " << each_item.name;
        }
        std::wcout << L'\n';
    }
}


void ReviseArguments() {

    //Use default output file path if it is absent.
    if (g_options.output_file_path.empty()) {
        g_options.output_file_path =
            g_working_directory /
            (L"zrc-" + std::to_wstring(std::time(nullptr)) + L".rc");
    }

    PrintParsedArguments();
}


void ReplaceSpaceInResourceName(std::wstring& name) {
    zaf::Replace(name, L" ", L"?20");
}


void ReplaceSlashInResourceName(std::wstring& name) {

    if (g_options.use_back_slash) {
        zaf::Replace(name, L"/", L"\\");
    }
    else {
        zaf::Replace(name, L"\\", L"/");
    }
}


void GenerateInputResourceItemsInDirectory(
    const std::filesystem::path& directory_path, 
    const std::wstring& name,
    std::vector<ResourceItem>& result) {

    std::wstring directory_path_string = directory_path.wstring();

    for (auto each_file : std::filesystem::recursive_directory_iterator(directory_path)) {

        if (each_file.is_directory()) {
            continue;
        }

        if (!g_options.included_extensions.empty()) {

            auto extension = each_file.path().extension();
            auto found = std::find(
                g_options.included_extensions.begin(),
                g_options.included_extensions.end(),
                extension);

            if (found == g_options.included_extensions.end()) {
                continue;
            }
        }

        auto resource_name = each_file.path().wstring();
        if (!name.empty()) {
            resource_name.replace(0, directory_path_string.length(), name);
        }
        else {
            resource_name.erase(0, directory_path_string.length() + 1);
        }
        ReplaceSlashInResourceName(resource_name);
        ReplaceSpaceInResourceName(resource_name);

        ResourceItem resource_item;
        resource_item.name = std::move(resource_name);
        resource_item.path = each_file.path();
        result.push_back(std::move(resource_item));
    }
}


std::vector<ResourceItem> GenerateInputResourceItems() {

    std::vector<ResourceItem> result;

    for (const auto& each_item : g_input_items) {

        std::filesystem::path full_path;
        if (each_item.path.is_absolute()) {
            full_path = each_item.path;
        }
        else {
            full_path = g_working_directory / each_item.path;
        }

        if (std::filesystem::is_directory(full_path)) {
            GenerateInputResourceItemsInDirectory(full_path, each_item.name, result);
        }
        else {

            ResourceItem resource_item;
            resource_item.path = full_path;

            if (!each_item.name.empty()) {
                resource_item.name = each_item.name;
            }
            else {
                resource_item.name = full_path.filename().wstring();
            }
            ReplaceSlashInResourceName(resource_item.name);
            ReplaceSpaceInResourceName(resource_item.name);

            result.push_back(resource_item);
        }
    }

    return result;
}


ResourceItem ParseResourceItem(const std::wstring& line) {

    ResourceItem result;

    auto first_delimiter_index = line.find(L' ');
    if (first_delimiter_index != std::string::npos) {

        result.name = line.substr(0, first_delimiter_index);

        auto second_delimiter_idnex = line.find(' ', first_delimiter_index + 1);
        if (second_delimiter_idnex != std::string::npos) {
            
            std::wistringstream path_stream(line.substr(second_delimiter_idnex + 1));
            path_stream >> result.path;
            return result;
        }
    }

    std::wcout << L"Invalid line in output file: \"" << line << L"\"." << std::endl;
    EXIT;
}


std::vector<ResourceItem> ReadResourceItemsFromOutputFile() {

    std::ifstream input_stream(g_options.output_file_path, std::ios::in | std::ios::binary);
    if (!input_stream) {
        std::wcout << L"Open output file failed." << std::endl;
        EXIT;
    }

    input_stream.seekg(0, std::ios::end);
    auto file_length = input_stream.tellg();

    if ((file_length < 2) || 
        (file_length % 2 != 0)) {
        std::wcout << L"Invalid encoding in output file. Expect UTF-16 LE with BOM." << std::endl;
        EXIT;
    }

    //Subtract 2 to exclude the BOM.
    std::size_t content_length = static_cast<std::size_t>(file_length) - 2;

    std::wstring content;
    content.resize(content_length / 2);

    input_stream.seekg(2, std::ios::beg);
    input_stream.read(reinterpret_cast<char*>(&content[0]), content_length);

    if (input_stream.gcount() != content_length) {
        std::wcout << L"Read output file failed." << std::endl;
        EXIT;
    }

    zaf::Replace(content, L"\r\n", L"\n");
    auto lines = zaf::Split(content, L'\n');

    std::vector<ResourceItem> result;
    for (const auto& each_line : lines) {

        //Skip empty lines.
        if (each_line.empty()) {
            continue;
        }

        //Skip comment lines.
        if (each_line.find(L"//") == 0) {
            continue;
        }

        result.push_back(ParseResourceItem(each_line));
    }

    return result;
}


bool CheckIfNeedReWriteResourceItems(
    std::vector<ResourceItem>& input_items, 
    std::vector<ResourceItem>& existent_items) {

    if (input_items.size() != existent_items.size()) {
        std::wcout << L"There are " << input_items.size() << L" input items and " 
            << existent_items.size() << L" existing items in output file." << std::endl;
        return true;
    }

    std::sort(input_items.begin(), input_items.end());
    std::sort(existent_items.begin(), existent_items.end());

    std::vector<ResourceItem> intersection;
    std::set_intersection(
        input_items.begin(),
        input_items.end(),
        existent_items.begin(),
        existent_items.end(),
        std::back_inserter(intersection));

    std::size_t difference_count = input_items.size() - intersection.size();
    if (difference_count > 0) {
        std::wcout << L"There are " << difference_count 
            << L" different items between "
            << L"input items and existing items in output file." << std::endl;
        return true;
    }

    std::wcout << L"There is no any difference between "
        << L"input items and existing items in output file." << std::endl;
    return false;
}


std::vector<ResourceItem> GenerateResourceItems() {

    auto input_resource_items = GenerateInputResourceItems();

    if (g_options.use_incremental_build &&
        std::filesystem::exists(g_options.output_file_path)) {
    
        auto existent_resource_items = ReadResourceItemsFromOutputFile();

        if (!CheckIfNeedReWriteResourceItems(input_resource_items, existent_resource_items)) {
            return {};
        }
    }

    std::sort(input_resource_items.begin(), input_resource_items.end());
    return input_resource_items;
}


void Output(const std::vector<ResourceItem>& resource_items) {

    std::wstringstream content_stream;
    for (const auto& each_item : resource_items) {

        content_stream
            << each_item.name 
            << L" ZRC "
            << each_item.path
            << L"\r\n";
    }

    std::ofstream output_stream(
        g_options.output_file_path,
        std::ios::out | std::ios::trunc | std::ios::binary);

    if (!output_stream) {
        std::wcout << L"Open output file failed." << std::endl;
        EXIT;
    }

    //Write UTF16 BOM.
    output_stream << std::uint8_t{ 0xff } << std::uint8_t{ 0xfe };

    auto content = content_stream.str();
    output_stream.write(
        reinterpret_cast<const char*>(content.data()), 
        sizeof(wchar_t) * content.length());

    output_stream.close();
}


int wmain(int argc, wchar_t** argv) {

    try {

        ParseArguments(argc, argv);
        if (g_options.print_help) {
            PrintHelp();
            return 0;
        }

        InitializeWorkingDirectory();
        ReviseArguments();

        auto resource_items = GenerateResourceItems();
        if (!resource_items.empty()) {

            std::wcout << L"Write " << resource_items.size() << L" items to output file." << std::endl;
            Output(resource_items);
            std::wcout << L"Done." << std::endl;
        }
        else {
            std::wcout << L"No need to update output file." << std::endl;
        }

        return 0;
    }
    catch (const std::exception& exception) {

        const char* what = exception.what();
        if (std::strlen(what) > 0) {
            std::cout << "Exception: " << what << std::endl;
        }
        return 1;
    }
}


