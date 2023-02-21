#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class HandleCreatedInfo : public EventInfo {
public:
    explicit HandleCreatedInfo(const std::shared_ptr<Window>& source);
};

}