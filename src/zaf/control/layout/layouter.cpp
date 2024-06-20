#include <zaf/control/layout/layouter.h>

namespace zaf {
namespace {

class SimpleLayouter : public Layouter {
public:
    SimpleLayouter(const LayoutFunction& layout_function) : layout_function_(layout_function) {

    }

    void Layout(
        const Control& parent,
        const Rect& parent_old_rect,
        const std::vector<std::shared_ptr<Control>>& children
    ) override {

        layout_function_(parent, parent_old_rect, children);
    }

private:
    LayoutFunction layout_function_;
};

}

ZAF_OBJECT_IMPL(Layouter)


void Layouter::Layout(
    const Control& parent,
    const Rect& parent_old_rect,
    const std::vector<std::shared_ptr<Control>>& children) {

    //Do nothing.
}


std::shared_ptr<Layouter> CreateLayouter(const LayoutFunction& layout_function) {
    return std::make_shared<SimpleLayouter>(layout_function);
}

}