#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {

class ControlUpdateLock {
public:
    explicit ControlUpdateLock(Control& owner) : owner_(owner) { }

    ~ControlUpdateLock() {
        owner_.EndUpdate();
    }

private:
    Control& owner_;
};


class ControlUpdateState {
public:
    Rect need_repainted_rect;
    bool need_relayout{};
    bool need_resize{};
};

}