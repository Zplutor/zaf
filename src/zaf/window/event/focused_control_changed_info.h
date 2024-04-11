#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;
class Window;

class FocusedControlChangedInfo : public EventInfo {
public:
    FocusedControlChangedInfo(
        std::shared_ptr<Window> source,
        std::shared_ptr<Control> previous_focused_control);

    const std::shared_ptr<Control>& PreviousFocusedControl() const {
        return previous_focused_control_;
    }

private:
    std::shared_ptr<Control> previous_focused_control_;
};

}