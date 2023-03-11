#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

namespace internal {

class MouseCaptureEventInfo : public EventInfo {
public:
    explicit MouseCaptureEventInfo(const std::shared_ptr<Control>& source);
};

}

using MouseCapturedInfo = internal::MouseCaptureEventInfo;
using MouseReleasedInfo = internal::MouseCaptureEventInfo;

}