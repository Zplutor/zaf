#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/base/as.h>
#include <zaf/control/text_box.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {

ZAF_OBJECT_IMPL(DynamicInlineObject);


std::shared_ptr<TextBox> DynamicInlineObject::Host() const noexcept {
    return As<TextBox>(__super::Host());
}


void DynamicInlineObject::NeedRepaint() {

    auto text_box = Host();
    if (text_box) {
        text_box->NeedRepaint();
    }
}


bool DynamicInlineObject::IsMouseOver() const {
    return is_mouse_over_;
}


bool DynamicInlineObject::IsInSelectionRange() const {
    
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


void DynamicInlineObject::OnDetached(const DetachedInfo& event_info) {

    is_mouse_over_ = false;

    __super::OnDetached(event_info);
}


bool DynamicInlineObject::HitTest(bool is_mouse_inside) {
    return true;
}


void DynamicInlineObject::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

}


void DynamicInlineObject::OnMouseEnter(const MouseEnterInfo& event_info) {

    is_mouse_over_ = true;

    mouse_enter_event_.Raise(event_info);
}


void DynamicInlineObject::OnMouseLeave(const MouseLeaveInfo& event_info) {

    is_mouse_over_ = false;

    mouse_leave_event_.Raise(event_info);
}


void DynamicInlineObject::OnMouseDown(const MouseDownInfo& event_info) {

    mouse_down_event_.Raise(event_info);
}


void DynamicInlineObject::OnMouseUp(const MouseUpInfo& event_info) {

    mouse_up_event_.Raise(event_info);
}


void DynamicInlineObject::OnDoubleClick(const DoubleClickInfo& event_info) {

    double_click_event_.Raise(event_info);
}


void DynamicInlineObject::WriteToXML(XMLWriter& writer) const {
    writer.WriteElementStart(L"DynamicInlineObject");
    writer.WriteElementEnd();
}


void DynamicInlineObject::ReadFromXML(XMLReader& reader) {
    auto [is_empty] = reader.ReadElementStart(L"DynamicInlineObject");
    if (!is_empty) {
        reader.ReadElementEnd();
    }
}

}