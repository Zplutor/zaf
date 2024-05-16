#include <gtest/gtest.h>
#include <zaf/clipboard/data_descriptor.h>

using namespace zaf::clipboard;

TEST(FormatTest, Equal) {

    {
        auto format1 = DataDescriptor::FromFormatType(FormatType::Text, MediumType::GlobalMem);
        auto format2 = DataDescriptor::FromFormatType(FormatType::Text, MediumType::GlobalMem);
        ASSERT_TRUE(format1 == format2);
    }

    {
        auto format1 = DataDescriptor::FromFormatType(FormatType::Text, MediumType::GlobalMem);
        FORMATETC format2{};
        format2.cfFormat = CF_UNICODETEXT;
        format2.tymed = TYMED_HGLOBAL;
        format2.dwAspect = DVASPECT_CONTENT;
        format2.lindex = -1;
        ASSERT_TRUE(format1 == format2);
        ASSERT_TRUE(format2 == format1);
    }
}


TEST(FormatTest, NotEqual) {

    auto format1 = DataDescriptor::FromFormatType(FormatType::Text, MediumType::GlobalMem);

    {
        FORMATETC format2 = format1.Inner();
        format2.cfFormat = CF_DIB;
        ASSERT_FALSE(format1 == format2);
    }

    {
        FORMATETC format2 = format1.Inner();
        format2.dwAspect = DVASPECT_ICON;
        ASSERT_FALSE(format1 == format2);
    }

    {
        FORMATETC format2 = format1.Inner();
        format2.lindex = 0;
        ASSERT_FALSE(format1 == format2);
    }

    {
        FORMATETC format2 = format1.Inner();
        DVTARGETDEVICE device{};
        format2.ptd = &device;
        ASSERT_FALSE(format1 == format2);
    }

    {
        FORMATETC format2 = format1.Inner();
        format2.tymed = TYMED_FILE;
        ASSERT_FALSE(format1 == format2);
    }
}