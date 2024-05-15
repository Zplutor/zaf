#include <gtest/gtest.h>
#include <zaf/base/error/invalid_data_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/clipboard/drop_files_data.h>

using namespace zaf;
using namespace zaf::clipboard;

TEST(DropFilesDataTest, MediumTransfer) {

    auto test = [](const std::vector<std::filesystem::path>& paths) {
    
        DropFilesData source_data{ paths };
        auto medium = source_data.SaveToMedium(Format{ FormatType::DropFiles });

        DropFilesData destination_data;
        destination_data.LoadFromMedium(FormatType::DropFiles, medium);

        return destination_data.GetFilePaths() == paths;
    };

    ASSERT_TRUE(test({ L"C:\\Windows\\Notepad.exe" }));
    ASSERT_TRUE(test({
        L"C:\\Windows\\Notepad.exe",
        L"C:\\Program Files\\RunAnywhere.exe"
    }));
}


TEST(DropFilesDataTest, SaveToMediumWithInvalidValue) {

    //Empty files.
    {
        DropFilesData data;
        ASSERT_THROW(data.SaveToMedium(Format(FormatType::DropFiles)), InvalidDataError);
    }

    //Not rooted path.
    {
        DropFilesData data{ { L"Windows\\notepad.exe" }};
        ASSERT_THROW(data.SaveToMedium(Format(FormatType::DropFiles)), InvalidDataError);
    }
}


TEST(DropFilesDataTest, SaveToMediumWithUnsupportedArguments) {

    DropFilesData data{ { L"C:\\Windows\\Notepad.exe" } };

    ASSERT_THROW(data.SaveToMedium(Format(FormatType::Text)), InvalidOperationError);
    ASSERT_THROW(
        data.SaveToMedium(Format(FormatType::DropFiles, MediumType::File)), 
        InvalidOperationError);
}


TEST(DropFilesDataTest, LoadFromMediumWithUnsupportedArguments) {

    auto global_mem = GlobalMem::Alloc(10, GlobalMemFlags::Movable);
    auto medium = Medium::FromGlobalMem(std::move(global_mem));

    DropFilesData data;
    ASSERT_THROW(
        data.LoadFromMedium(FormatType::Text, medium),
        InvalidOperationError);

    ASSERT_THROW(
        data.LoadFromMedium(FormatType::DropFiles, {}),
        InvalidOperationError);
}