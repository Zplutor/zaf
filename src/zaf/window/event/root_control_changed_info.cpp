#include <zaf/window/event/root_control_changed_info.h>
#include <zaf/window/window.h>

namespace zaf {

RootControlChangedInfo::RootControlChangedInfo(
    const std::shared_ptr<Window>& source,
    const std::shared_ptr<Control>& previous_root_control)
    :
    EventInfo(source),
    previous_root_control_(previous_root_control) {

}

}