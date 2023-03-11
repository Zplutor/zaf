#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;
class Window;

class MouseCaptureControlChangedInfo : public EventInfo {
public:
    explicit MouseCaptureControlChangedInfo(
        const std::shared_ptr<Window>& source,
        const std::shared_ptr<Control>& previous_control);

    /**
    Previous control that captured mouse, may be nullptr.
    */
    const std::shared_ptr<Control>& PreviousControl() const {
        return previous_control_;
    }

private:
    std::shared_ptr<Control> previous_control_;
};

}