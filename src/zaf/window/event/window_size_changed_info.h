#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class WindowSizeChangedInfo : public EventInfo {
public:
    explicit WindowSizeChangedInfo(const std::shared_ptr<Window>& window);
};

}