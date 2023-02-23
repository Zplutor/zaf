#include <zaf/window/event/show_info.h>
#include <zaf/window/window.h>

namespace zaf {

ShowInfo::ShowInfo(
    const std::shared_ptr<Window>& source,
    const ShowWindowMessage& message)
    : 
    EventInfo(source),
    message_(message.Inner()) {

}

}