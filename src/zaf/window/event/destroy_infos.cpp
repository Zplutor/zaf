#include <zaf/window/event/destroy_infos.h>
#include <zaf/window/window.h>

namespace zaf {

DestroyingInfo::DestroyingInfo(std::shared_ptr<Window> source, DestroyReason reason) noexcept :
    EventInfo(std::move(source)),
    reason_(reason) {

}


DestroyedInfo::DestroyedInfo(
    std::shared_ptr<Window> source,
    HWND window_handle,
    DestroyReason reason) noexcept
    :
    EventInfo(std::move(source)),
    window_handle_(window_handle),
    reason_(reason) {

}

}