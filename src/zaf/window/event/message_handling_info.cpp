#include <zaf/window/event/message_handling_info.h>
#include <zaf/window/window.h>

namespace zaf {

MessageHandlingInfo::MessageHandlingInfo(
    const std::shared_ptr<Window>& source,
    const zaf::Message& message)
    :
    EventInfo(source),
    state_(std::make_shared<internal::MessageHandlingSharedState>(message)) {

}

}