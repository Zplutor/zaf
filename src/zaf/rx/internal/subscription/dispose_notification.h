#pragma once

#include <functional>

namespace zaf::rx::internal {

using DisposeNotificationID = std::pair<std::uintptr_t, int>;
using DisposeNotification = std::function<void(DisposeNotificationID)>;

}