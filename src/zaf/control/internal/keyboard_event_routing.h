#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {

bool RouteKeyboardEvent(const std::shared_ptr<Control>& event_target, const Message& message);

}