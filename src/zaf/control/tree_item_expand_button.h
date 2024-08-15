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
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;

private:
    ExpandState expand_state_{ ExpandState::None };
};


}
