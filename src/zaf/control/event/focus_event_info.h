#pragma once

#include <zaf/base/event/routed_event_info.h>

namespace zaf {
namespace internal {

class FocusEventInfo : public RoutedEventInfo {
public:
    FocusEventInfo(
        const std::shared_ptr<RoutedEventSharedState>& state,
        const std::shared_ptr<Object>& sender);
};

}

using FocusGainedInfo = internal::FocusEventInfo;
using FocusLostInfo = internal::FocusEventInfo;

}