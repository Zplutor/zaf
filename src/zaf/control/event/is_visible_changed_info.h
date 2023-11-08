#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

class IsVisibleChangedInfo : public EventInfo {
public:
    explicit IsVisibleChangedInfo(std::shared_ptr<Control> source);
};

}