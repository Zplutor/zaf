#pragma once

#include <zaf/control/layout/layouter.h>

namespace zaf {

class StretchLayouter : public Layouter {
public:
    ZAF_OBJECT;

public:
    void Layout(
        const Control& parent,
        const Size& parent_old_size,
        const std::vector<std::shared_ptr<Control>>& children) override;
};

ZAF_OBJECT_BEGIN(StretchLayouter)
ZAF_OBJECT_END;

}