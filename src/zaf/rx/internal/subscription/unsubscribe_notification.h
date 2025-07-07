#pragma once

#include <functional>

namespace zaf::rx::internal {

using UnsubscribeNotificationID = std::pair<std::uintptr_t, int>;
using UnsubscribeNotification = std::function<void(UnsubscribeNotificationID)>;

}