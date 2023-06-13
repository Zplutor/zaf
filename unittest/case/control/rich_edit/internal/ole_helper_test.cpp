#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/control/rich_edit.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/rich_edit/internal/ole_helper.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

using namespace zaf::rich_edit;
using namespace zaf::rich_edit::internal;

namespace {

class OLEObject : public EmbeddedObject {
public:
    OLEObject() {
        SetSize(zaf::Size{ 10,10 });
    }

    GUID ClassID() const override {
        return { 0x8e2986c6, 0xd51d, 0x4af8, { 0x87, 0x8a, 0xbe, 0x35, 0x9e, 0x25, 0x4d, 0xc3 } };
    }

    void Paint(
        zaf::Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const zaf::rich_edit::PaintContext& context) override {

    }
};


class OLEHelperTest : public testing::Test {
protected:
    void SetUp() override {

        window_ = zaf::Create<zaf::Window>();
        window_->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
        window_->SetRect(zaf::Rect{ 0, 0, 100, 100 });
        window_->SetHasBorder(false);
        window_->SetIsPopup(true);

        rich_edit_ = zaf::Create<zaf::RichEdit>();
        rich_edit_->SetBorder(zaf::Frame{ 0 });

        window_->SetRootControl(rich_edit_);
        window_->Show();
    }

    void TearDown() override {
        window_->Destroy();
    }

    const std::shared_ptr<zaf::RichEdit>& GetRichEdit() const {
        return rich_edit_;
    }

private:
    std::shared_ptr<zaf::Window> window_;
    std::shared_ptr<zaf::RichEdit> rich_edit_;
};

}

TEST_F(OLEHelperTest, FindObjectAtScreenPosition_SingleObjectOnly) {

    const auto& rich_edit = GetRichEdit();
    rich_edit->InsertObject(zaf::Create<OLEObject>());

    auto test = [&rich_edit](const zaf::Point& screen_position) {
        auto position_in_pixels = zaf::FromDIPs(screen_position, rich_edit->GetDPI()).ToPOINT();
        auto object_info = OLEHelper::FindObjectAtScreenPosition(*rich_edit, position_in_pixels);
        if (object_info.object) {
            return object_info.text_position == 0;
        }
        return false;
    };

    //Position is inside the object.
    ASSERT_TRUE(test(zaf::Point(0, 0)));
    ASSERT_TRUE(test(zaf::Point(4, 0)));
    ASSERT_TRUE(test(zaf::Point(5, 0)));
    ASSERT_TRUE(test(zaf::Point(9, 0)));

    ASSERT_TRUE(test(zaf::Point(0, 5)));
    ASSERT_TRUE(test(zaf::Point(4, 5)));
    ASSERT_TRUE(test(zaf::Point(5, 5)));
    ASSERT_TRUE(test(zaf::Point(9, 5)));

    ASSERT_TRUE(test(zaf::Point(0, 9)));
    ASSERT_TRUE(test(zaf::Point(4, 9)));
    ASSERT_TRUE(test(zaf::Point(5, 9)));
    ASSERT_TRUE(test(zaf::Point(9, 9)));

    //Position is outside the object horizontally.
    ASSERT_FALSE(test(zaf::Point(-1, 0)));
    ASSERT_FALSE(test(zaf::Point(10, 0)));
    ASSERT_FALSE(test(zaf::Point(11, 0)));

    ASSERT_FALSE(test(zaf::Point(-1, 5)));
    ASSERT_FALSE(test(zaf::Point(10, 5)));
    ASSERT_FALSE(test(zaf::Point(11, 5)));

    ASSERT_FALSE(test(zaf::Point(-1, 9)));
    ASSERT_FALSE(test(zaf::Point(10, 9)));
    ASSERT_FALSE(test(zaf::Point(11, 9)));
}


TEST_F(OLEHelperTest, FindObjectAtScreenPosition_ObjectAlongWithText) {

    const auto& rich_edit = GetRichEdit();
    rich_edit->ReplaceSelectedText(L"----");
    auto preceding_text_width = rich_edit->CalculatePreferredSize().width;
    preceding_text_width = std::ceil(preceding_text_width);
    rich_edit->InsertObject(zaf::Create<OLEObject>());
    rich_edit->ReplaceSelectedText(L"++++");

    auto test = [&rich_edit](const zaf::Point& screen_position) {
        auto position_in_pixels = zaf::FromDIPs(screen_position, rich_edit->GetDPI()).ToPOINT();
        auto object_info = OLEHelper::FindObjectAtScreenPosition(*rich_edit, position_in_pixels);
        return
            object_info.object &&
            object_info.text_position == 4;
    };

    //Position is inside the object.
    ASSERT_TRUE(test(zaf::Point(preceding_text_width, 0)));
    ASSERT_TRUE(test(zaf::Point(preceding_text_width + 4, 0)));
    ASSERT_TRUE(test(zaf::Point(preceding_text_width + 5, 0)));
    ASSERT_TRUE(test(zaf::Point(preceding_text_width + 8, 0)));

    //Position is outside the object.
    ASSERT_FALSE(test(zaf::Point(0, 0)));
    ASSERT_FALSE(test(zaf::Point(preceding_text_width / 2, 0)));
    ASSERT_FALSE(test(zaf::Point(preceding_text_width - 2, 0)));
    ASSERT_FALSE(test(zaf::Point(preceding_text_width + 10, 0)));
}


TEST_F(OLEHelperTest, FindObjectAtScreenPosition_AdjacentObjects) {

    const auto& rich_edit = GetRichEdit();
    rich_edit->InsertObject(zaf::Create<OLEObject>());
    rich_edit->InsertObject(zaf::Create<OLEObject>());

    auto test = [&rich_edit](
        const zaf::Point& screen_position, 
        std::size_t text_position) {

        auto position_in_pixels = zaf::FromDIPs(screen_position, rich_edit->GetDPI()).ToPOINT();
        auto object_info = OLEHelper::FindObjectAtScreenPosition(*rich_edit, position_in_pixels);
        return
            object_info.object &&
            object_info.text_position == text_position;
    };

    //Find the first object.
    ASSERT_TRUE(test(zaf::Point(0, 0), 0));
    ASSERT_TRUE(test(zaf::Point(5, 0), 0));
    ASSERT_TRUE(test(zaf::Point(9, 0), 0));

    //Find the second object.
    ASSERT_TRUE(test(zaf::Point(10, 0), 1));
    ASSERT_TRUE(test(zaf::Point(15, 0), 1));
    ASSERT_TRUE(test(zaf::Point(19, 0), 1));
}