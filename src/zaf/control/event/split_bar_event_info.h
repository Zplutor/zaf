#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class SplitBar;

namespace internal {

class SplitBarEventInfo : public EventInfo {
public:
    explicit SplitBarEventInfo(const std::shared_ptr<SplitBar>& source);
};

}

using SplitBarBeginDragInfo = internal::SplitBarEventInfo;
using SplitBarDragInfo = internal::SplitBarEventInfo;
using SplitBarEndDragInfo = internal::SplitBarEventInfo;

}