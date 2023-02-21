#include <zaf/window/event/destroyed_info.h>
#include <zaf/window/window.h>

namespace zaf {

DestroyedInfo::DestroyedInfo(
    const std::shared_ptr<Window>& source,
    HWND original_window_handle) 
    :
    EventInfo(source),
    original_window_handle_(original_window_handle) {

}

}