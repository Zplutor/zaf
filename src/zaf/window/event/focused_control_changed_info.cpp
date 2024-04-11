#include <zaf/window/event/focused_control_changed_info.h>
#include <zaf/window/window.h>

namespace zaf {

FocusedControlChangedInfo::FocusedControlChangedInfo(
    std::shared_ptr<Window> source,
    std::shared_ptr<Control> previous_focused_control)
    :
    EventInfo(std::move(source)),
    previous_focused_control_(std::move(previous_focused_control)) {

}

}