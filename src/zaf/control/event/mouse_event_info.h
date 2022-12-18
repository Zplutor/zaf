#pragma once

#include <zaf/base/event/routed_event_info.h>

namespace zaf {

class Control;

namespace internal {

class MouseOverInfo : public RoutedEventInfo {
public:
    MouseOverInfo(const std::shared_ptr<Control>& source);
};

}

using MouseEnterInfo = internal::MouseOverInfo;
using MouseLeaveInfo = internal::MouseOverInfo;
using MouseHoverInfo = internal::MouseOverInfo;

}