#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf::textual {

class InlineObject;

class MouseCursorChangingInfo : public EventInfo {
public:
    explicit MouseCursorChangingInfo(std::shared_ptr<InlineObject> source);
};

}
