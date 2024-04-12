#include <zaf/internal/control/control_focus_event_info_helpers.h>
#include <zaf/control/control.h>

namespace zaf::internal {

FocusEventSharedState::FocusEventSharedState(
    std::shared_ptr<Control> source,
    std::shared_ptr<Control> changing_control)
    :
    RoutedEventSharedState(std::move(source)),
    changing_control_(std::move(changing_control)) {

}

}