#pragma once

#include <zaf/base/event/routed_event_info.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {

class Control;

namespace internal {

class KeyboardEventSharedState : public RoutedEventSharedState {
public:
    KeyboardEventSharedState(const std::shared_ptr<Control>& source, const zaf::Message& message);

    const zaf::Message& Message() const {
        return message_;
    }

private:
    zaf::Message message_;
};


template<typename MessageShim, int>
class KeyboardEventInfo : public RoutedEventInfo {
public:
    KeyboardEventInfo(
        const std::shared_ptr<KeyboardEventSharedState>& state,
        const std::shared_ptr<Control>& sender) 
        :
        RoutedEventInfo(state, sender),
        state_(state) {

    }

    MessageShim Message() const {
        return MessageShim{ state_->Message() };
    }

private:
    std::shared_ptr<KeyboardEventSharedState> state_;
};

}

using PreKeyDownInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;
using KeyDownInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;

using PreKeyUpInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;
using KeyUpInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;

using PreCharInputInfo = internal::KeyboardEventInfo<CharMessage, __LINE__>;
using CharInputInfo = internal::KeyboardEventInfo<CharMessage, __LINE__>;

using PreSysKeyDownInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;
using SysKeyDownInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;

using PreSysKeyUpInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;
using SysKeyUpInfo = internal::KeyboardEventInfo<KeyMessage, __LINE__>;

using PreSysCharInputInfo = internal::KeyboardEventInfo<CharMessage, __LINE__>;
using SysCharInputInfo = internal::KeyboardEventInfo<CharMessage, __LINE__>;

}