#include <gtest/gtest.h>
#include <fstream>
#include <zaf/base/string/replace.h>
#include "test_helper.h"

namespace {

std::wstring BuildInputResourcePath(const std::wstring& relative_path) {

    auto full_path = GetInputDirectoryPath() / relative_path;

    std::wstring result;
    result.append(1, L'"');
    result.append(full_path);
    result.append(1, L'"');

    zaf::Replace(result, L"\\", L"\\\\");
    return result;
}


std::filesystem::path GetIncrementalOutputFilePath() {
    return GetWorkingDirectoryPath() / L"incremental.rc";
}


void CreateFileInInputDirectory(const std::wstring& file_name) {

    auto file_path = GetInputDirectoryPath() / file_name;
    std::ofstream file_stream(file_path, std::ios::out);
}

}


class TestZrcb : public testing::Test {
public:
    void SetUp() override {

        auto working_directory = GetWorkingDirectoryPath();
        std::filesystem::remove_all(working_directory);

        auto template_path = GetTemplateDirectoryPath();
        std::filesystem::copy(
            template_path,
            working_directory,
            std::filesystem::copy_options::recursive);
    }
};


TEST_F(TestZrcb, DefaultOptions) {

    auto input_path = GetInputDirectoryPath();

    RunZrcb(L"\"" + input_path.wstring() + L"\"");
    ASSERT_TRUE(CheckOutputRC(FindDefaultOutputRCFile(), {
        { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
        { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
        { L"other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
        { L"other_files/中文/文档.txt", BuildInputResourcePath(L"other_files\\中文\\文档.txt") },
        { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
        { L"xaml?20files/xaml?202.xaml", BuildInputResourcePath(L"xaml files\\xaml 2.xaml") },
    }));
}


TEST_F(TestZrcb, SpecifyNameForDirectory) {

    std::wostringstream argument_stream;
    argument_stream << L"\"" << GetInputDirectoryPath().wstring() << L"\""
        << L"*zrcb";

    RunZrcb(argument_stream.str());

    ASSERT_TRUE(CheckOutputRC(FindDefaultOutputRCFile(), {
        { L"zrcb/file1.txt", BuildInputResourcePath(L"file1.txt") },
        { L"zrcb/file2.txt", BuildInputResourcePath(L"file2.txt") },
        { L"zrcb/other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
        { L"zrcb/other_files/中文/文档.txt", BuildInputResourcePath(L"other_files\\中文\\文档.txt") },
        { L"zrcb/xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
        { L"zrcb/xaml?20files/xaml?202.xaml", BuildInputResourcePath(L"xaml files\\xaml 2.xaml") },
    }));
}


TEST_F(TestZrcb, Output) {

    auto test = [](const std::wstring_view& option) {
    
        auto output_file_path = GetWorkingDirectoryPath() / L"test_output_option.rc";

        std::wstring arguments;
        arguments += option;
        arguments += L" \"";
        arguments += output_file_path.wstring();
        arguments += L"\" \"";
        arguments += GetInputDirectoryPath().wstring();
        arguments += L"\"";

        RunZrcb(arguments);
        return CheckOutputRC(
            output_file_path, 
            {
                { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
                { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
                { L"other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
                { 
                    L"other_files/中文/文档.txt", 
                    BuildInputResourcePath(L"other_files\\中文\\文档.txt") 
                },
                { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
                {
                    L"xaml?20files/xaml?202.xaml", 
                    BuildInputResourcePath(L"xaml files\\xaml 2.xaml") 
                },
            }
        );
    };

    ASSERT_TRUE(test(L"/out"));
    ASSERT_TRUE(test(L"/o"));
}


TEST_F(TestZrcb, BackSlash) {

    auto test = [](const std::wstring_view& option) {
    
        auto input_path = GetInputDirectoryPath();

        std::wstring arguments;
        arguments += option;
        arguments += L" \"";
        arguments += input_path.wstring();
        arguments += L"\"";

        RunZrcb(arguments);
        return CheckOutputRC(FindDefaultOutputRCFile(), {
            { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
            { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
            { L"other_files\\json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
            { 
                L"other_files\\中文\\文档.txt", 
                BuildInputResourcePath(L"other_files\\中文\\文档.txt") 
            },
            { L"xaml?20files\\xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
            { L"xaml?20files\\xaml?202.xaml", BuildInputResourcePath(L"xaml files\\xaml 2.xaml") },
        });
    };

    ASSERT_TRUE(test(L"/back-slash"));
    ASSERT_TRUE(test(L"/b"));
}


TEST_F(TestZrcb, Extension) {

    auto test = [](const std::wstring_view& option) {

        auto input_path = GetInputDirectoryPath();

        std::wstring arguments;
        arguments += option;
        arguments += L" .json.xaml \"";
        arguments += input_path.wstring();
        arguments += L"\"";

        RunZrcb(arguments);
        return CheckOutputRC(FindDefaultOutputRCFile(), {
            { L"other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
            { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
            { L"xaml?20files/xaml?202.xaml", BuildInputResourcePath(L"xaml files\\xaml 2.xaml") },
        });
    };

    ASSERT_TRUE(test(L"/extension"));
    ASSERT_TRUE(test(L"/e"));
}


TEST_F(TestZrcb, IncrementalNoChanges) {

    auto test = [](const std::wstring_view& option) {
    
        auto incremental_file_path = GetIncrementalOutputFilePath();

        std::wostringstream argument_stream;
        argument_stream 
            << L"/out \"" << incremental_file_path << L"\" "
            << option << L' '
            << GetInputDirectoryPath().wstring();

        RunZrcb(argument_stream.str());
        return CheckOutputRC(
            incremental_file_path, 
            {
                { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
                { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
                { L"other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
                {
                    L"other_files/中文/文档.txt",
                    BuildInputResourcePath(L"other_files\\中文\\文档.txt")
                },
                { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
                {
                    L"xaml?20files/xaml?202.xaml",
                    BuildInputResourcePath(L"xaml files\\xaml 2.xaml")
                },
            });
    };

    ASSERT_TRUE(test(L"/incremental"));
    ASSERT_TRUE(test(L"/i"));
}


TEST_F(TestZrcb, IncrementalAddFiles) {

    CreateFileInInputDirectory(L"inc.jpg");
    CreateFileInInputDirectory(L"inc.png");

    auto incremental_file_path = GetIncrementalOutputFilePath();

    std::wostringstream argument_stream;
    argument_stream
        << L"/i /out \"" << incremental_file_path << L"\" "
        << GetInputDirectoryPath().wstring();

    RunZrcb(argument_stream.str());

    ASSERT_TRUE(CheckOutputRC(
        incremental_file_path,
        {
            { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
            { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
            { L"inc.jpg", BuildInputResourcePath(L"inc.jpg") },
            { L"inc.png", BuildInputResourcePath(L"inc.png") },
            { L"other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
            {
                L"other_files/中文/文档.txt",
                BuildInputResourcePath(L"other_files\\中文\\文档.txt")
            },
            { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
            {
                L"xaml?20files/xaml?202.xaml",
                BuildInputResourcePath(L"xaml files\\xaml 2.xaml")
            },
        }));
}


TEST_F(TestZrcb, IncrementalRemoveFiles) {

    std::filesystem::remove(GetInputDirectoryPath() / L"other_files" / L"中文" / L"文档.txt");
    std::filesystem::remove(GetInputDirectoryPath() / L"other_files" / L"json1.json");
    std::filesystem::remove(GetInputDirectoryPath() / L"xaml files" / L"xaml 2.xaml");

    auto incremental_file_path = GetIncrementalOutputFilePath();

    std::wostringstream argument_stream;
    argument_stream
        << L"/i /out \"" << incremental_file_path << L"\" "
        << GetInputDirectoryPath().wstring();

    RunZrcb(argument_stream.str());

    ASSERT_TRUE(CheckOutputRC(
        incremental_file_path,
        {
            { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
            { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
            { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
        }));
}


