#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf::textual {

class DynamicInlineObject;

class MouseCursorChangingInfo : public EventInfo {
public:
    explicit MouseCursorChangingInfo(std::shared_ptr<DynamicInlineObject> source);

    bool IsHandled() const {
        return *is_handled_;
    }

    void MarkAsHandled() const {
        *is_handled_ = true;
    }

private:
    std::shared_ptr<bool> is_handled_;
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