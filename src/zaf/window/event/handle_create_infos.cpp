#include <zaf/window/event/handle_create_infos.h>
#include <zaf/window/window.h>

namespace zaf {

HandleCreatingInfo::HandleCreatingInfo(std::shared_ptr<Window> source) noexcept :
    EventInfo(std::move(source)) {

}


HandleCreatedInfo::HandleCreatedInfo(std::shared_ptr<Window> source) noexcept : 
    EventInfo(std::move(source)) {

}

}