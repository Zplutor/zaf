#include <zaf/control/layout/layouter.h>

namespace zaf {
namespace {

class SimpleLayouter : public Layouter {
public:
    SimpleLayouter(const LayoutFunction& layout_function) : layout_function_(layout_function) {

    }

    void Layout(
        const Control& parent,
        const Size& parent_old_size,
        const std::vector<std::shared_ptr<Control>>& children
    ) override {

        layout_function_(parent, parent_old_size, children);
    }

private:
    LayoutFunction layout_function_;
};

}

ZAF_OBJECT_IMPL(Layouter)


void Layouter::Layout(
    const Control& parent,
    const Size& parent_old_size,
    const std::vector<std::shared_ptr<Control>>& children) {

    //Do nothing.
}


std::shared_ptr<Layouter> CreateLayouter(const LayoutFunction& layout_function) {
    return std::make_shared<SimpleLayouter>(layout_function);
}

}