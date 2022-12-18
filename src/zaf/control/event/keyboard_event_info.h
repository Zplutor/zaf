#pragma once

#include <zaf/base/event/routed_event_info.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {
namespace internal {

template<typename MessageShim>
class KeyboardEventInfo : public RoutedEventInfo {
public:
    KeyboardEventInfo(const std::shared_ptr<Object>& source, const Message& message) :
        RoutedEventInfo(source),
        message_(message) {

    }

    MessageShim Message() const {
        return MessageShim{ message_ };
    }

private:
    zaf::Message message_;
};

}

using KeyDownInfo = internal::KeyboardEventInfo<KeyMessage>;
using KeyUpInfo = internal::KeyboardEventInfo<KeyMessage>;
using CharInputInfo = internal::KeyboardEventInfo<CharMessage>;

}