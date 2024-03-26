#include <zaf/control/textual/ranged_text_style.h>

namespace zaf::textual {

RangedTextStyle RangedTextStyle::Clone() const {

    RangedTextStyle result;
    result.fonts_ = this->fonts_;
    result.text_color_pickers_ = this->text_color_pickers_;
    result.text_back_color_pickers_ = this->text_back_color_pickers_;
    result.inline_objects_ = this->inline_objects_.Clone();
    return result;
}

}