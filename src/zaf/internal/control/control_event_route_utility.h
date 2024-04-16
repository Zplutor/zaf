#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {

std::vector<std::shared_ptr<Control>> BuildTunnelPath(
    const std::shared_ptr<Control>& event_target);

}