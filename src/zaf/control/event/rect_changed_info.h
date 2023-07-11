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


class PositionChangedInfo : public EventInfo {
public:
    PositionChangedInfo(const std::shared_ptr<Control>& source, const Point& previous_position);

    const Point& PreviousPosition() const {
        return previous_position_;
    }

private:
    Point previous_position_;
};


class SizeChangedInfo : public EventInfo {
public:
    SizeChangedInfo(const std::shared_ptr<Control>& source, const Size& previous_size);

    const Size& PreviousSize() const {
        return previous_size_;
    }

private:
    Size previous_size_;
};

}