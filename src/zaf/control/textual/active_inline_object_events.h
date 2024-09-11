#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace textual {
class ActiveInlineObject;
}

namespace internal {

class HandleableInlineObjectEventInfo : public EventInfo {
public:
    explicit HandleableInlineObjectEventInfo(std::shared_ptr<textual::ActiveInlineObject> source);

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
        std::shared_ptr<textual::ActiveInlineObject> source, 
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

class InlineObjectMouseEnterInfo : public EventInfo {
public:
    explicit InlineObjectMouseEnterInfo(std::shared_ptr<ActiveInlineObject> source);
};


class InlineObjectMouseLeaveInfo : public EventInfo {
public:
    explicit InlineObjectMouseLeaveInfo(std::shared_ptr<ActiveInlineObject> source);
};


class InlineObjectMouseCursorChangingInfo : public internal::HandleableInlineObjectEventInfo {
public:
    using HandleableInlineObjectEventInfo::HandleableInlineObjectEventInfo;
};


class InlineObjectDoubleClickInfo : public internal::HandleableInlineObjectEventInfo {
public:
    using HandleableInlineObjectEventInfo::HandleableInlineObjectEventInfo;
};


using InlineObjectMouseDownInfo = internal::InlineObjectMouseEventInfo<__LINE__>;
using InlineObjectMouseUpInfo = internal::InlineObjectMouseEventInfo<__LINE__>;

}
}