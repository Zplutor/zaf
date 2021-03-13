#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Options {
public:
    std::filesystem::path output_file_path;
    std::vector<std::string> included_extensions;
    bool use_incremental_build{};
    bool use_back_slash{};
    bool print_help{};
};

class InputItem {
public:
    std::filesystem::path path;
    std::string name;
};

class ResourceItem {
public:
    std::string name;
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

    std::cout <<
R"(Zaf rc builder

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


bool IsHelpOption(const std::string_view& argument) {
    return argument == "/help" || argument == "/h";
}

bool IsOuptutFileOption(const std::string_view& argument) {
    return argument == "/out" || argument == "/o";
}

bool IsExtensionOption(const std::string_view& argument) {
    return argument == "/extension" || argument == "/e";
}

bool IsIncrementalBuildOption(const std::string_view& argument) {
    return argument == "/incremental" || argument == "/i";
}

bool IsBackSlashOption(const std::string_view& argument) {
    return argument == "/back-slash" || argument == "/b";
}

bool IsOptionArgument(const std::string_view& argument) {
    return argument.front() == '/';
}


std::vector<std::string> ParseExtensions(const std::string_view& argument) {

    if (argument.front() != '.') {
        std::cout << "Invalid option value \"" << argument << "\".";
        EXIT;
    }

    std::vector<std::string> result;

    std::size_t current_index = 0;
    while (current_index < argument.length()) {

        auto next_dot_index = argument.find('.', current_index + 1);

        auto extension = argument.substr(current_index, next_dot_index - current_index);
        result.push_back(std::string{ extension.data(), extension.length() });

        current_index = next_dot_index;
    }

    return result;
}


void ParseInputItem(const std::string_view& argument) {

    InputItem input_item;

    auto delimiter_index = argument.find('*');
    if (delimiter_index == std::string_view::npos) {
        input_item.path = argument;
    }
    else {
        input_item.path = argument.substr(0, delimiter_index);
        input_item.name = argument.substr(delimiter_index + 1);
    }

    if (input_item.path.empty()) {
        std::cout << "Invalid argument \"" << argument << "\"." << std::endl;
        EXIT;
    }

    g_input_items.push_back(input_item);
}


void ParseArguments(int argc, char** argv) {

    if (argc < 2) {
        g_options.print_help = true;
        return;
    }

    int index = 1;
    while (index < argc) {

        const char* argument = argv[index];

        //Help
        if (IsHelpOption(argument)) {
            g_options.print_help = true;
            ++index;
        }
        //Output file path
        else if (IsOuptutFileOption(argument)) {

            int next_argument_index = index + 1;
            if (next_argument_index >= argc) {
                std::cout << "A file path is needed for option \"" << argument << "\"." 
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
                std::cout << "An extension list is needed for option \"" << argument << "\"."
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

            std::cout << "Unknown option \"" << argument << "\"." << std::endl;
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
        std::cout << "Get working directory failed with error code " << GetLastError << '.'
            << std::endl;
        EXIT;
    }

    g_working_directory = buffer;
    std::cout << "Working directory: " << g_working_directory << std::endl;
}


void PrintParsedArguments() {

    std::cout
        << "Output file path: " << g_options.output_file_path << '\n'
        << "Use incremental build: " << g_options.use_incremental_build << '\n'
        << "Use back slash: " << g_options.use_back_slash << '\n';

    std::cout << "Included extensions: " << '\n';
    for (const auto& each_extension : g_options.included_extensions) {
        std::cout << each_extension << '\n';
    }

    std::cout << "Input items: " << '\n';
    for (const auto& each_item : g_input_items) {

        std::cout << each_item.path;
        if (!each_item.name.empty()) {
            std::cout << " -> " << each_item.name;
        }
        std::cout << '\n';
    }
}


void ReviseArguments() {

    //Use default output file path if it is absent.
    if (g_options.output_file_path.empty()) {
        g_options.output_file_path =
            g_working_directory /
            ("zrc-" + std::to_string(std::time(nullptr)) + ".rc");
    }

    PrintParsedArguments();
}


void ReplaceSlashInResourceName(std::string& name) {

    if (g_options.use_back_slash) {

        std::transform(name.begin(), name.end(), name.begin(), [](char ch) {
            if (ch == '/') {
                return '\\';
            }
            return ch;
        });
    }
    else {

        std::transform(name.begin(), name.end(), name.begin(), [](char ch) {
            if (ch == '\\') {
                return '/';
            }
            return ch;
        });
    }
}


void GenerateInputResourceItemsInDirectory(
    const std::filesystem::path& directory_path, 
    const std::string& name,
    std::vector<ResourceItem>& result) {

    std::string directory_path_string = directory_path.string();

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

        auto resource_name = each_file.path().string();
        if (!name.empty()) {
            resource_name.replace(0, directory_path_string.length(), name);
        }
        else {
            resource_name.erase(0, directory_path_string.length() + 1);
        }
        ReplaceSlashInResourceName(resource_name);

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
            resource_item.path = std::move(full_path);

            if (!each_item.name.empty()) {
                resource_item.name = each_item.name;
            }
            else {
                resource_item.name = full_path.filename().string();
            }
            ReplaceSlashInResourceName(resource_item.name);

            result.push_back(resource_item);
        }
    }

    return result;
}


ResourceItem ParseResourceItem(const std::string& line) {

    ResourceItem result;

    auto first_delimiter_index = line.find(' ');
    if (first_delimiter_index != std::string::npos) {

        result.name = line.substr(0, first_delimiter_index);

        auto second_delimiter_idnex = line.find(' ', first_delimiter_index + 1);
        if (second_delimiter_idnex != std::string::npos) {
            
            std::istringstream path_stream(line.substr(second_delimiter_idnex + 1));
            path_stream >> result.path;
            return result;
        }
    }

    std::cout << "Invalid line in output file: \"" << line << "\"." << std::endl;
    EXIT;
}


std::vector<ResourceItem> ReadResourceItemsFromOutputFile() {

    std::ifstream input_stream(g_options.output_file_path, std::ios::in);
    if (!input_stream) {
        std::cout << "Open output file failed.";
        EXIT;
    }

    std::vector<ResourceItem> result;

    std::string line;
    while (std::getline(input_stream, line)) {
        result.push_back(ParseResourceItem(line));
    }

    return result;
}


bool CheckIfNeedReWriteResourceItems(
    std::vector<ResourceItem>& input_items, 
    std::vector<ResourceItem>& existent_items) {

    if (input_items.size() != existent_items.size()) {
        std::cout << "There are " << input_items.size() << " input items and " 
            << existent_items.size() << " existing items in output file." << std::endl;
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
        std::cout << "There are " << difference_count 
            << " different items between "
            << "input items and existing items in output file." << std::endl;
        return true;
    }

    std::cout << "There is no any difference between "
        << "input items and existing items in output file." << std::endl;
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

    std::ofstream output_stream(g_options.output_file_path, std::ios::out | std::ios::trunc);
    if (!output_stream) {
        std::cout << "Open output file failed." << std::endl;
        EXIT;
    }

    for (const auto& each_item : resource_items) {

        output_stream 
            << each_item.name << ' ' 
            << "ZRC" << ' ' 
            << each_item.path 
            << "\n";
    }

    output_stream.close();
}


int main(int argc, char** argv) {

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

            std::cout << "Write " << resource_items.size() << " items to output file." << std::endl;
            Output(resource_items);
            std::cout << "Done." << std::endl;
        }
        else {
            std::cout << "No need to update output file." << std::endl;
        }

        return 0;
    }
    catch (const std::exception& exception) {

        const char* what = exception.what();
        if (std::strlen(what) > 0) {
            std::cout << what << std::endl;
        }
        return 1;
    }
}


