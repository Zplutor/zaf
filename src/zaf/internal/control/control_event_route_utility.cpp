#include <zaf/internal/control/control_event_route_utility.h>

namespace zaf::internal {

std::vector<std::shared_ptr<Control>> BuildTunnelPath(
    const std::shared_ptr<Control>& event_target) {

    std::vector<std::shared_ptr<Control>> result;
    auto current = event_target;
    while (current) {
        result.push_back(current);
        current = current->Parent();
    }

    std::reverse(result.begin(), result.end());
    return result;
}

}