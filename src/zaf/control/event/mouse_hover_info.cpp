#include <zaf/control/event/mouse_hover_info.h>
#include <zaf/control/control.h>

namespace zaf {

MouseHoverInfo::MouseHoverInfo(
    const std::shared_ptr<RoutedEventSharedState>& state,
    const std::shared_ptr<Control>& sender) 
    :
    RoutedEventInfo(state, sender) {

}

}