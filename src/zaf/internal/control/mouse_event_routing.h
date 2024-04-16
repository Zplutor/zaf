#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {

class MouseEventTargetInfo {
public:
    std::shared_ptr<Control> control;
    Point position;
};

MouseEventTargetInfo FindMouseEventTarget(
    const std::shared_ptr<Control>& begin_control,
    const Point& position_at_begin_control);

void TunnelMouseEvent(
    const std::shared_ptr<Control>& event_target, 
    const std::shared_ptr<MouseEventSharedState>& event_info_state,
    const MouseMessage& message);


void BubbleMouseEvent(
    const std::shared_ptr<Control>& event_target,
    const Point& position_at_event_target,
    const std::shared_ptr<MouseEventSharedState>& event_info_state,
    const MouseMessage& message);

}