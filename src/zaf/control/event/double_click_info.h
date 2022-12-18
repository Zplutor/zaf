#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/graphic/point.h>

namespace zaf {

class Control;

class DoubleClickInfo : public EventInfo {
public:
    DoubleClickInfo(const std::shared_ptr<Control>& source, const Point& position);

    const Point& Position() const {
        return position_;
    }

private:
    Point position_;
};

}