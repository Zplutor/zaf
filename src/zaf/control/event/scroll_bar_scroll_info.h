#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ScrollBar;

class ScrollBarScrollInfo : public EventInfo {
public:
    explicit ScrollBarScrollInfo(const std::shared_ptr<ScrollBar>& source);
};

}