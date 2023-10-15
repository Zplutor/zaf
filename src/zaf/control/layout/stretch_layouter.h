#pragma once

#include <zaf/control/layout/layouter.h>

namespace zaf {

class StretchLayouter : public Layouter {
public:
    ZAF_DECLARE_TYPE;

public:
    void Layout(
        const Control& parent,
        const Rect& parent_old_rect,
        const std::vector<std::shared_ptr<Control>>& children) override;
};

}