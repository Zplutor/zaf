#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class RichEdit;

class RichEditSelectionChangedInfo : public EventInfo {
public:
    explicit RichEditSelectionChangedInfo(const std::shared_ptr<RichEdit>& source);
};

}