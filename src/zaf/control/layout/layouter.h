#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <zaf/object/object.h>

namespace zaf {

class Control;
class Rect;

class Layouter : public Object {
public:
    ZAF_DECLARE_TYPE

    virtual void Layout(
        const Control& parent,
        const Rect& parent_old_rect,
        const std::vector<std::shared_ptr<Control>>& children);
};


using LayoutFunction = std::function<void(
    const Control & parent,
    const Rect & parent_old_rect,
    const std::vector<std::shared_ptr<Control>> & children
)>;

std::shared_ptr<Layouter> CreateLayouter(const LayoutFunction& layout_function);

}