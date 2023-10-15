#include <zaf/control/layout/stretch_layouter.h>
#include <zaf/control/control.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(StretchLayouter)
ZAF_DEFINE_TYPE_END

void StretchLayouter::Layout(
    const Control& parent,
    const Rect& parent_old_rect, 
    const std::vector<std::shared_ptr<Control>>& children) {

    if (children.empty()) {
        return;
    }

    Rect child_rect(Point(), parent.ContentSize());

    for (const auto& each_child : children) {
        each_child->SetRect(child_rect);
    }
}

}