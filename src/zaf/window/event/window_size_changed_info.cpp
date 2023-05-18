#include <zaf/window/event/window_size_changed_info.h>
#include <zaf/window/window.h>

namespace zaf {

WindowSizeChangedInfo::WindowSizeChangedInfo(const std::shared_ptr<Window>& window) :
    EventInfo(window) {

}

}