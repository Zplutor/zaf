#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/clipboard/text_data.h>

using namespace zaf;
using namespace zaf::clipboard;

TEST(TextDataTest, WriteToMedium) {

    TextData text_data{ L"TextDataTest" };

    auto format = DataDescriptor::FromFormatType(FormatType::Text, MediumType::GlobalMem);
    auto medium = text_data.WriteToMedium(format);
    ASSERT_EQ(medium.Type(), MediumType::GlobalMem);
    
    std::wstring text;
    medium.VisitGlobalMem([&text](const GlobalMem& global_mem) {
        auto lock = global_mem.Lock();
        text = reinterpret_cast<const wchar_t*>(lock.Pointer());
    });
    ASSERT_EQ(text, text_data.Text());
}


TEST(TextDataTest, SaveToMediumUnsupportedFormat) {

    TextData text_data{ L"TextDataTest" };
    
    auto format = DataDescriptor::FromFormatType(static_cast<FormatType>(0));
    ASSERT_THROW(text_data.WriteToMedium(format), InvalidOperationError);
}


TEST(TextDataTest, ReadFromMedium) {

    constexpr const wchar_t* TestString = L"ReadFromMedium";

    auto buffer_size = (std::wcslen(TestString) + 1) * sizeof(wchar_t);
    auto global_mem = GlobalMem::Alloc(buffer_size, GlobalMemFlags::Movable);
    {
        auto lock = global_mem.Lock();
        std::memcpy(lock.Pointer(), TestString, buffer_size);
    }

    auto medium = Medium::FromGlobalMem(std::move(global_mem));

    TextData text_data;
    text_data.ReadFromMedium(medium, DataDescriptor::FromFormatType(FormatType::Text));

    ASSERT_EQ(text_data.Text(), TestString);
}


TEST(TextDataTest, LoadFromMediumUnsupportedFormat) {

    TextData text_data;
    ASSERT_THROW(
        text_data.ReadFromMedium({}, DataDescriptor::FromFormatType(FormatType::Indeterminate)),
        InvalidOperationError);
}