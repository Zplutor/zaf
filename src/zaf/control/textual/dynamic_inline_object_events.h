#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace textual {
class DynamicInlineObject;
}

namespace internal {

class HandleableInlineObjectEventInfo : public EventInfo {
public:
    explicit HandleableInlineObjectEventInfo(std::shared_ptr<textual::DynamicInlineObject> source);

    bool IsHandled() const {
        return *is_handled_;
    }

    void MarkAsHandled() const {
        *is_handled_ = true;
    }

private:
    std::shared_ptr<bool> is_handled_;
};


template<int>
class InlineObjectMouseEventInfo : public HandleableInlineObjectEventInfo {
public:
    InlineObjectMouseEventInfo(
        std::shared_ptr<textual::DynamicInlineObject> source, 
        const MouseMessage& message)
        : 
        HandleableInlineObjectEventInfo(std::move(source)),
        message_(message.Inner()) {

    }

    MouseMessage Message() const noexcept {
        return MouseMessage{ message_ };
    }

private:
    zaf::Message message_;
};

}

namespace textual {

class MouseEnterInfo : public EventInfo {
public:
    explicit MouseEnterInfo(std::shared_ptr<DynamicInlineObject> source);
};


class MouseLeaveInfo : public EventInfo {
public:
    explicit MouseLeaveInfo(std::shared_ptr<DynamicInlineObject> source);
};


class MouseCursorChangingInfo : public internal::HandleableInlineObjectEventInfo {
public:
    using HandleableInlineObjectEventInfo::HandleableInlineObjectEventInfo;
};


using MouseDownInfo = internal::InlineObjectMouseEventInfo<__LINE__>;
using MouseUpInfo = internal::InlineObjectMouseEventInfo<__LINE__>;

}
}