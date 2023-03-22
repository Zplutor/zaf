#pragma once

#include <zaf/base/event/routed_event_info.h>

namespace zaf {

class Control;

class MouseHoverInfo : public RoutedEventInfo {
public:
    MouseHoverInfo(
        const std::shared_ptr<RoutedEventSharedState>& state,
        const std::shared_ptr<Control>& sender);
};

}