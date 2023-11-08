#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

class IsEnabledChangedInfo : public EventInfo {
public:
    explicit IsEnabledChangedInfo(std::shared_ptr<Control> source);
};

}