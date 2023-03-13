#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;
class Window;

class RootControlChangedInfo : public EventInfo {
public:
    explicit RootControlChangedInfo(
        const std::shared_ptr<Window>& source,
        const std::shared_ptr<Control>& previous_root_control);

    //Previous root control. It is nullptr for the first time the window initializes.
    const std::shared_ptr<Control>& PreviousRootControl() const {
        return previous_root_control_;
    }

private:
    std::shared_ptr<Control> previous_root_control_;
};

}