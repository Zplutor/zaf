#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/object/type_definition.h>

namespace zaf::textual {

ZAF_DEFINE_TYPE(DynamicInlineObject)
ZAF_DEFINE_TYPE_END;

bool DynamicInlineObject::IsMouseOver() const {
    return false;
}


bool DynamicInlineObject::IsInSelectionRange() const {
    return false;
}


void DynamicInlineObject::OnMouseCursorChanging(
    const textual::MouseCursorChangingInfo& event_info) {

}


void DynamicInlineObject::OnMouseDown() {

}


void DynamicInlineObject::OnMouseUp() {

}

}