#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

class IsSelectedChangedInfo : public EventInfo {
public:
    explicit IsSelectedChangedInfo(std::shared_ptr<Control> source);
};

}