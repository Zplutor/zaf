#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class CheckBox;

class CheckStateChangedInfo : public EventInfo {
public:
    explicit CheckStateChangedInfo(const std::shared_ptr<CheckBox>& source);
};

}