#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ScrollBarArrow;

namespace internal {

class ScrollBarArrowPressInfo : public EventInfo {
public:
    explicit ScrollBarArrowPressInfo(const std::shared_ptr<ScrollBarArrow>& source);
};

}

using ScrollBarArrowBeginPressInfo = internal::ScrollBarArrowPressInfo;
using ScrollBarArrowEndPressInfo = internal::ScrollBarArrowPressInfo;

}