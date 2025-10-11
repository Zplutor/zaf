#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class HandleCreatingInfo : public EventInfo {
public:
    explicit HandleCreatingInfo(std::shared_ptr<Window> source) noexcept;
};


class HandleCreatedInfo : public EventInfo {
public:
    explicit HandleCreatedInfo(std::shared_ptr<Window> source) noexcept;
};

}