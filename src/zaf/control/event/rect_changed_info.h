#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class Control;

class RectChangedInfo : public EventInfo {
public:
    RectChangedInfo(const std::shared_ptr<Control>& source, const Rect& previous_rect);

    const Rect& PreviousRect() const {
        return previous_rect_;
    }

private:
    Rect previous_rect_;
};

}