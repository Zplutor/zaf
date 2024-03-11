#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf::textual {

class DynamicInlineObject;

class MouseCursorChangingInfo : public EventInfo {
public:
    explicit MouseCursorChangingInfo(std::shared_ptr<DynamicInlineObject> source);
};


class MouseEnterInfo : public EventInfo {
public:
    explicit MouseEnterInfo(std::shared_ptr<DynamicInlineObject> source);
};


class MouseLeaveInfo : public EventInfo {
public:
    explicit MouseLeaveInfo(std::shared_ptr<DynamicInlineObject> source);
};

}