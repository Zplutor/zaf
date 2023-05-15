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
    rich_edit->InsertObject(zaf::MakeCOMObject<TestEmbeddedObject>());
    ASSERT_EQ(rich_edit->TextLength(), 3);
}