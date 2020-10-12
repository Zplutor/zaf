#pragma once

#include <zaf/control/button.h>
#include <zaf/control/expand_state.h>

namespace zaf {

class TreeItemExpandButton : public Button {
public:
    ExpandState GetExpandState() const {
        return expand_state_;
    }

    void SetExpandState(ExpandState state);

protected:
    void Initialize();
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;

private:
    ExpandState expand_state_{ ExpandState::None };
};


}
