#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class TextualControl;

class TextChangedInfo : public EventInfo {
public:
    explicit TextChangedInfo(const std::shared_ptr<TextualControl>& source);
};

}