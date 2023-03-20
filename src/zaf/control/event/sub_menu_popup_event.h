#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class MenuItem;

namespace internal {

class SubMenuEventInfo : public EventInfo {
public:
    explicit SubMenuEventInfo(const std::shared_ptr<MenuItem>& source);
};

}

using SubMenuShowInfo = internal::SubMenuEventInfo;
using SubMenuCloseInfo = internal::SubMenuEventInfo;

}