#include <zaf/control/textual/interactive_inline_object.h>
#include <zaf/base/as.h>
#include <zaf/control/text_box.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {

ZAF_OBJECT_IMPL(InteractiveInlineObject);


std::shared_ptr<TextBox> InteractiveInlineObject::Host() const noexcept {
    return As<TextBox>(__super::Host());
}


void InteractiveInlineObject::NeedRepaint() {

    auto text_box = Host();
    if (text_box) {
        text_box->NeedRepaint();
    }
}


std::optional<Point> InteractiveInlineObject::PositionInHost() const {

    auto host = Host();
    if (!host) {
        return std::nullopt;
    }

    auto range_in_host = RangeInHost();
    if (!range_in_host) {
        return std::nullopt;
    }

    auto hit_test_result = host->HitTestAtIndex(range_in_host->index);
    return hit_test_result.Rect().position;
}


bool InteractiveInlineObject::IsMouseOver() const noexcept {
    return is_mouse_over_;
}


bool InteractiveInlineObject::IsInSelectionRange() const noexcept {
    
    auto host = Host();
    if (!host) {
        return false;
    }

    auto range_in_host = RangeInHost();
    if (!range_in_host) {
        return false;
    }

    return host->SelectionRange().Contains(*range_in_host);
}


void InteractiveInlineObject::OnDetached(const DetachedInfo& event_info) {

    is_mouse_over_ = false;

    __super::OnDetached(event_info);
}


bool InteractiveInlineObject::HitTest(bool is_mouse_inside) {
    return true;
}


void InteractiveInlineObject::OnMouseCursorChanging(
    const InlineObjectMouseCursorChangingInfo& event_info) {

}


void InteractiveInlineObject::OnMouseEnter(const InlineObjectMouseEnterInfo& event_info) {

    is_mouse_over_ = true;

    mouse_enter_event_.Raise(event_info);
}


void InteractiveInlineObject::OnMouseLeave(const InlineObjectMouseLeaveInfo& event_info) {

    is_mouse_over_ = false;

    mouse_leave_event_.Raise(event_info);
}


void InteractiveInlineObject::OnMouseDown(const InlineObjectMouseDownInfo& event_info) {

    mouse_down_event_.Raise(event_info);
}


void InteractiveInlineObject::OnMouseUp(const InlineObjectMouseUpInfo& event_info) {

    mouse_up_event_.Raise(event_info);
}


void InteractiveInlineObject::OnDoubleClick(const InlineObjectDoubleClickInfo& event_info) {

    double_click_event_.Raise(event_info);
}


void InteractiveInlineObject::WriteToXML(XMLWriter& writer) const {
    writer.WriteElementStart(L"InteractiveInlineObject");
    writer.WriteElementEnd();
}


void InteractiveInlineObject::ReadFromXML(XMLReader& reader) {
    auto [is_empty] = reader.ReadElementStart(L"InteractiveInlineObject");
    if (!is_empty) {
        reader.ReadElementEnd();
    }
}

}