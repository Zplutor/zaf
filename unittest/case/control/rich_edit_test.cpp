#include <gtest/gtest.h>
#include <zaf/control/rich_edit.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/creation.h>

namespace {

class TestEmbeddedObject : public zaf::rich_edit::EmbeddedObject {
public:
    GUID ClassID() const override {
        return { 0x1e293bee, 0xcd07, 0x4e02, { 0xa6, 0x14, 0xec, 0xe6, 0x98, 0xf4, 0xe3, 0xef } };

    }

    void Paint(
        zaf::Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const zaf::rich_edit::PaintContext& context) override {

    }
};

}

TEST(RichEditTest, TextLength) {

    auto rich_edit = zaf::Create<zaf::RichEdit>();
    ASSERT_EQ(rich_edit->TextLength(), 0);

    //Single line
    rich_edit->SetText(L"abµÄcd");
    ASSERT_EQ(rich_edit->TextLength(), 5);

    //Multiple lines
    rich_edit->SetIsMultiline(true);
    rich_edit->SetText(L"a\r\nb\r\nc\r\n");
    ASSERT_EQ(rich_edit->TextLength(), 6); // "\r\n" will be replaced to "\r"

    //Embedded object
    rich_edit->SetText(L"11");
    rich_edit->InsertObject(zaf::Create<TestEmbeddedObject>());
    ASSERT_EQ(rich_edit->TextLength(), 3);
}


TEST(RichEditTest, GetText) {

    auto rich_edit = zaf::Create<zaf::RichEdit>();
    rich_edit->SetIsMultiline(true);
    rich_edit->SetText(L"line1\rline2\r\nline3\r\n");

    auto text = rich_edit->GetText(zaf::rich_edit::TextFlag::Default);
    ASSERT_EQ(text, L"line1\rline2\rline3\r");

    text = rich_edit->GetText(zaf::rich_edit::TextFlag::UseCRLF);
    ASSERT_EQ(text, L"line1\r\nline2\r\nline3\r\n");
}


TEST(RichEditTest, CalculatePreferrenceSize) {

    //Multi-line and word wrapping could cause bad result.
    //Make sure it has been resolved.
    auto rich_edit = zaf::Create<zaf::RichEdit>();
    rich_edit->SetFontSize(10);
    rich_edit->SetWordWrapping(zaf::WordWrapping::Wrap);
    rich_edit->SetIsMultiline(true);
    rich_edit->SetBorder(zaf::Frame{});
    rich_edit->SetText(L"abcdefghijklmn");

    auto result = rich_edit->CalculatePreferredSize();
    ASSERT_GT(result.width, 20);
}