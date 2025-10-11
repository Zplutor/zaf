#include <zaf/window/event/handle_create_infos.h>
#include <zaf/window/window.h>

namespace zaf {

HandleCreatingInfo::HandleCreatingInfo(std::shared_ptr<Window> source, HWND window_handle) :
    EventInfo(std::move(source)),
    window_handle_(window_handle) {

}


HandleCreatedInfo::HandleCreatedInfo(const std::shared_ptr<Window>& source) : EventInfo(source) {

}

}