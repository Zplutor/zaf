#include <zaf/window/event/show_window_event_info.h>
#include <zaf/window/window.h>

namespace zaf::internal {

ShowWindowEventInfo::ShowWindowEventInfo(
    const std::shared_ptr<Window>& source,
    const ShowWindowMessage& message)
    : 
    EventInfo(source),
    message_(message.Inner()) {

}

}