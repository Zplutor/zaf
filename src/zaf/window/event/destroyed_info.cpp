#include <zaf/window/event/destroyed_info.h>
#include <zaf/window/window.h>

namespace zaf {

DestroyedInfo::DestroyedInfo(
    std::shared_ptr<Window> source,
    HWND original_window_handle,
    DestroyReason reason)
    :
    EventInfo(std::move(source)),
    original_window_handle_(original_window_handle),
    reason_(reason) {

}

}