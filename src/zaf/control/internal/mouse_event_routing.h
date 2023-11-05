#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {

bool RouteMouseEvent(
    const std::shared_ptr<Control>& begin_routing_control,
    const Point& position_at_begin_routing_control,
    const MouseMessage& message);

}