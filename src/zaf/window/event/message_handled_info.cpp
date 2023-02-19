#include <zaf/window/event/message_handled_info.h>
#include <zaf/window/window.h>

namespace zaf {

MessageHandledInfo::MessageHandledInfo(
    const std::shared_ptr<Window>& source,
    const zaf::Message& message,
    LRESULT handle_result)
    :
    EventInfo(source),
    message_(message),
    handle_result_(handle_result) {

}

}