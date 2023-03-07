#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ListControl;

class ListControlSelectionChangedInfo : public EventInfo {
public:
    explicit ListControlSelectionChangedInfo(const std::shared_ptr<ListControl>& source);
};

}