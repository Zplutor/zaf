#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <zaf/object/object.h>

namespace zaf {

class Control;
class Size;

class Layouter : public Object {
public:
    ZAF_OBJECT;

    virtual void Layout(
        const Control& parent,
        const Size& parent_old_size,
        const std::vector<std::shared_ptr<Control>>& children);
};

ZAF_OBJECT_BEGIN(Layouter);
ZAF_OBJECT_END;


using LayoutFunction = std::function<void(
    const Control & parent,
    const Size & parent_old_size,
    const std::vector<std::shared_ptr<Control>> & children
)>;

std::shared_ptr<Layouter> CreateLayouter(const LayoutFunction& layout_function);

}