#include <gtest/gtest.h>
#include <zaf/base/error/not_supported_error.h>
#include <zaf/clipboard/text_data.h>

using namespace zaf;
using namespace zaf::clipboard;

TEST(TextDataTest, SaveToMedium) {

    TextData text_data{ L"TextDataTest" };

    Format format{ FormatType::Text, MediumType::GlobalMem };
    auto medium = text_data.SaveToMedium(format);
    ASSERT_EQ(medium.Type(), MediumType::GlobalMem);
    
    std::wstring text;
    medium.VisitGlobalMem([&text](const GlobalMem& global_mem) {
        auto lock = global_mem.Lock();
        text = reinterpret_cast<const wchar_t*>(lock.Pointer());
    });
    ASSERT_EQ(text, text_data.GetText());
}


TEST(TextDataTest, SaveToMediumUnsupportedFormat) {

    TextData text_data{ L"TextDataTest" };
    
    Format format{ static_cast<FormatType>(0) };
    ASSERT_THROW(text_data.SaveToMedium(format), NotSupportedError);
}


TEST(TextDataTest, LoadFromMedium) {

    constexpr const wchar_t* TestString = L"LoadFromMedium";

    auto buffer_size = (std::wcslen(TestString) + 1) * sizeof(wchar_t);
    auto global_mem = GlobalMem::Alloc(buffer_size, GlobalMemFlags::Movable);
    {
        auto lock = global_mem.Lock();
        std::memcpy(lock.Pointer(), TestString, buffer_size);
    }

    auto medium = Medium::FromGlobalMem(std::move(global_mem));

    TextData text_data;
    text_data.LoadFromMedium(Format{ FormatType::Text }, medium);

    ASSERT_EQ(text_data.GetText(), TestString);
}


TEST(TextDataTest, LoadFromMediumUnsupportedFormat) {

    TextData text_data;
    ASSERT_THROW(text_data.LoadFromMedium(Format{}, {}), NotSupportedError);
}