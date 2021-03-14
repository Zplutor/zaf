#include <gtest/gtest.h>
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

}


class TestZrcb : public testing::Test {
public:
    void SetUp() override {

        PrepareInputDirectory();
        CleanOutputDirectory();
    }

private:
    void PrepareInputDirectory() {

        auto template_path = GetTemplateDirectoryPath();
        auto input_path = GetInputDirectoryPath();

        std::filesystem::remove_all(input_path);
        std::filesystem::copy(template_path, input_path, std::filesystem::copy_options::recursive);
    }

    void CleanOutputDirectory() {

        auto output_path = GetOutputDirectoryPath();
        std::filesystem::remove_all(output_path);
        std::filesystem::create_directories(output_path);
    }
};


TEST_F(TestZrcb, DefaultOptions) {

    auto input_path = GetInputDirectoryPath();

    RunZrcb(input_path.wstring());
    ASSERT_TRUE(CheckOutputRC(FindDefaultOutputRCFile(), {
        { L"file1.txt", BuildInputResourcePath(L"file1.txt") },
        { L"file2.txt", BuildInputResourcePath(L"file2.txt") },
        { L"other_files/json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
        { L"other_files/中文/文档.txt", BuildInputResourcePath(L"other_files\\中文\\文档.txt") },
        { L"xaml?20files/xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
        { L"xaml?20files/xaml?202.xaml", BuildInputResourcePath(L"xaml files\\xaml 2.xaml") },
    }));
}


TEST_F(TestZrcb, UseBackSlash) {

    auto test = [](const std::wstring_view& option) {
    
        auto input_path = GetInputDirectoryPath();

        std::wstring arguments;
        arguments += option;
        arguments += L' ';
        arguments += input_path.wstring();

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
        arguments += L" .json.xaml ";
        arguments += input_path.wstring();

        RunZrcb(arguments);
        return CheckOutputRC(FindDefaultOutputRCFile(), {
            { L"other_files\\json1.json", BuildInputResourcePath(L"other_files\\json1.json") },
            { L"xaml?20files\\xaml1.xaml", BuildInputResourcePath(L"xaml files\\xaml1.xaml") },
            { L"xaml?20files\\xaml?202.xaml", BuildInputResourcePath(L"xaml files\\xaml 2.xaml") },
        });
    };

    ASSERT_TRUE(test(L"/extension"));
    ASSERT_TRUE(test(L"/e"));
}