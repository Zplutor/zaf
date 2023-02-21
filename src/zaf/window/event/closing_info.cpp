#include <zaf/window/event/closing_info.h>
#include <zaf/window/window.h>

namespace zaf {

ClosingInfo::ClosingInfo(const std::shared_ptr<Window>& source) : 
    EventInfo(source),
    can_close_(std::make_shared<bool>(true)) {

}

}