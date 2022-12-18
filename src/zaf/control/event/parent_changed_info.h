#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

class ParentChangedInfo : public EventInfo {
public:
    ParentChangedInfo(
        const std::shared_ptr<Control>& source,
        const std::shared_ptr<Control>& previous_parent);

    const std::shared_ptr<Control>& PreviousParent() const {
        return previous_parent_;
    }

private:
    std::shared_ptr<Control> previous_parent_;
};

}