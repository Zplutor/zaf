#include <zaf/graphic/text/custom_text_inline_object.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(CustomTextInlineObject)
ZAF_DEFINE_TYPE_END;


void CustomTextInlineObject::Paint(Canvas& canvas) const {

}


TextInlineObjectMetrics CustomTextInlineObject::GetMetrics() const {
    return {};
}

}