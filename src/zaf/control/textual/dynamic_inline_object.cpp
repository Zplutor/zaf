#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/base/as.h>
#include <zaf/control/text_box.h>
#include <zaf/internal/textual/text_box_inline_object_manager.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/object/type_definition.h>

namespace zaf::textual {

ZAF_DEFINE_TYPE(DynamicInlineObject)
ZAF_DEFINE_TYPE_END;

void DynamicInlineObject::NeedRepaint() {

    auto text_box = As<TextBox>(Host());
    if (text_box) {
        text_box->NeedRepaint();
    }
}


bool DynamicInlineObject::IsMouseOver() const {
    return is_mouse_over_;
}


bool DynamicInlineObject::IsInSelectionRange() const {
    
    auto manager = GetManager();
    if (manager) {
        return manager->IsInlineObjectSelected(*this);
    }
    return false;
}


void DynamicInlineObject::OnDetached(const DetachedInfo& event_info) {

    is_mouse_over_ = false;

    __super::OnDetached(event_info);
}


HitTestResult DynamicInlineObject::HitTest(const HitTestInfo& info) {
    return HitTestResult::Object;
}


void DynamicInlineObject::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

}


void DynamicInlineObject::OnMouseEnter(const MouseEnterInfo& event_info) {

    is_mouse_over_ = true;
}


void DynamicInlineObject::OnMouseLeave(const MouseLeaveInfo& event_info) {

    is_mouse_over_ = false;
}


void DynamicInlineObject::OnMouseDown() {

}


void DynamicInlineObject::OnMouseUp() {

}


internal::TextBoxInlineObjectManager* DynamicInlineObject::GetManager() const {

    auto text_box = As<TextBox>(Host());
    if (text_box) {
        return &text_box->module_context_->InlineObjectManager();
    }
    return nullptr;
}

}