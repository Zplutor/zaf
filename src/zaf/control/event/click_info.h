#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ClickableControl;

class ClickInfo : public EventInfo {
public:
    explicit ClickInfo(const std::shared_ptr<ClickableControl>& source);
};

}