#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;
class Window;

class WindowChangedInfo : public EventInfo {
public:
    WindowChangedInfo(
        const std::shared_ptr<Control>& source,
        const std::shared_ptr<Window>& previous_window);

    const std::shared_ptr<Window>& PreviousWindow() const {
        return previous_window_;
    }

private:
    std::shared_ptr<Window> previous_window_;
};

}