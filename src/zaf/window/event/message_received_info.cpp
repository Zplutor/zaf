#include <zaf/window/event/message_received_info.h>
#include <zaf/window/window.h>

namespace zaf {

MessageReceivedInfo::MessageReceivedInfo(
    const std::shared_ptr<Window>& source,
    const zaf::Message& message)
    :
    EventInfo(source),
    state_(std::make_shared<internal::MessageReceivedSharedState>(message)) {

}

}