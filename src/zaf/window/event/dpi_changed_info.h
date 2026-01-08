#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class DPIChangedInfo : public EventInfo {
public:
    explicit DPIChangedInfo(const std::shared_ptr<Window>& window);
};

}
