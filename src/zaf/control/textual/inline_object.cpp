#include <zaf/control/textual/inline_object.h>
#include <zaf/object/type_definition.h>

namespace zaf::textual {

ZAF_DEFINE_TYPE(InlineObject)
ZAF_DEFINE_TYPE_END;


void InlineObject::Paint(Canvas& canvas) const {

}


TextInlineObjectMetrics InlineObject::GetMetrics() const {
    return {};
}


void InlineObject::OnMouseCursorChanging(
    const textual::MouseCursorChangingInfo& event_info) {

}


void InlineObject::OnMouseDown() {

}


void InlineObject::OnMouseUp() {
    
}

}