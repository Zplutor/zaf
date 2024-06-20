#include <zaf/control/layout/stretch_layouter.h>
#include <zaf/control/control.h>

namespace zaf {

ZAF_OBJECT_IMPL(StretchLayouter)

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