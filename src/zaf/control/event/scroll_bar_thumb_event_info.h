#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ScrollBarThumb;

namespace internal {

class ScrollBarThumbEventInfo : public EventInfo {
public:
    explicit ScrollBarThumbEventInfo(const std::shared_ptr<ScrollBarThumb>& source);
};

}

using ScrollBarThumbBeginDragInfo = internal::ScrollBarThumbEventInfo;
using ScrollBarThumbDragInfo = internal::ScrollBarThumbEventInfo;
using ScrollBarThumbEndDragInfo = internal::ScrollBarThumbEventInfo;

}
