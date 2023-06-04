#include <gtest/gtest.h>
#include <zaf/clipboard/drop_files_data.h>

using namespace zaf::clipboard;

TEST(DropFilesDataTest, MediumTransfer) {

    auto test = [](const std::vector<std::filesystem::path>& paths) {
    
        DropFilesData source_data{ paths };
        auto medium = source_data.SaveToMedium(Format{ FormatType::DropFiles });

        DropFilesData destination_data;
        destination_data.LoadFromMedium(Format{ FormatType::DropFiles }, medium);

        return destination_data.GetFilePaths() == paths;
    };

    ASSERT_TRUE(test({ L"C:\\Windows\\Notepad.exe" }));
    ASSERT_TRUE(test({
        L"C:\\Windows\\Notepad.exe",
        L"C:\\Program Files\\RunAnywhere.exe"
    }));
}