#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {
class ListCore;
}

namespace zaf {

/**
Represents a container control that acts as the parent of items in a list control.
*/
class ListItemContainer : public Control {
protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;
};

}