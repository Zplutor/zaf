#pragma once

#include <zaf/base/event/routed_event_info.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class Control;

class ControlEventInfo {
public:
    ControlEventInfo(const std::shared_ptr<Control>& control) : control_(control) {

    }

    const std::shared_ptr<Control>& Control() const {
        return control_;
    }

private:
    std::shared_ptr<zaf::Control> control_;
};


class ControlFocusChangeInfo : public ControlEventInfo {
public:
    using ControlEventInfo::ControlEventInfo;
};

}