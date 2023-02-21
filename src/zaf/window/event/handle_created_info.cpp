#include <zaf/window/event/handle_created_info.h>
#include <zaf/window/window.h>

namespace zaf {

HandleCreatedInfo::HandleCreatedInfo(const std::shared_ptr<Window>& source) : EventInfo(source) {

}

}