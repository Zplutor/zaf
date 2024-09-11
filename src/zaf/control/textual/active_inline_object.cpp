#include <zaf/control/textual/active_inline_object.h>
#include <zaf/base/as.h>
#include <zaf/control/text_box.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {

ZAF_OBJECT_IMPL(ActiveInlineObject);


std::shared_ptr<TextBox> ActiveInlineObject::Host() const noexcept {
    return As<TextBox>(__super::Host());
}


void ActiveInlineObject::NeedRepaint() {

    auto text_box = Host();
    if (text_box) {
        text_box->NeedRepaint();
    }
}


std::optional<Point> ActiveInlineObject::PositionInHost() const {

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


bool ActiveInlineObject::IsMouseOver() const noexcept {
    return is_mouse_over_;
}


bool ActiveInlineObject::IsInSelectionRange() const noexcept {
    
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


void ActiveInlineObject::OnDetached(const DetachedInfo& event_info) {

    is_mouse_over_ = false;

    __super::OnDetached(event_info);
}


bool ActiveInlineObject::HitTest(bool is_mouse_inside) {
    return true;
}


void ActiveInlineObject::OnMouseCursorChanging(
    const InlineObjectMouseCursorChangingInfo& event_info) {

}


void ActiveInlineObject::OnMouseEnter(const InlineObjectMouseEnterInfo& event_info) {

    is_mouse_over_ = true;

    mouse_enter_event_.Raise(event_info);
}


void ActiveInlineObject::OnMouseLeave(const InlineObjectMouseLeaveInfo& event_info) {

    is_mouse_over_ = false;

    mouse_leave_event_.Raise(event_info);
}


void ActiveInlineObject::OnMouseDown(const InlineObjectMouseDownInfo& event_info) {

    mouse_down_event_.Raise(event_info);
}


void ActiveInlineObject::OnMouseUp(const InlineObjectMouseUpInfo& event_info) {

    mouse_up_event_.Raise(event_info);
}


void ActiveInlineObject::OnDoubleClick(const InlineObjectDoubleClickInfo& event_info) {

    double_click_event_.Raise(event_info);
}


void ActiveInlineObject::WriteToXML(XMLWriter& writer) const {
    writer.WriteElementStart(L"ActiveInlineObject");
    writer.WriteElementEnd();
}


void ActiveInlineObject::ReadFromXML(XMLReader& reader) {
    auto [is_empty] = reader.ReadElementStart(L"ActiveInlineObject");
    if (!is_empty) {
        reader.ReadElementEnd();
    }
}

}