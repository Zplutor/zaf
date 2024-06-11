#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

class VisualStateUpdateInfo : public EventInfo {
public:
    explicit VisualStateUpdateInfo(const std::shared_ptr<Control>& control);
};

}