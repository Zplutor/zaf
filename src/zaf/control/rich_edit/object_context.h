#pragma once

#include <zaf/control/event/double_click_info.h>
#include <zaf/control/event/mouse_event_info.h>

namespace zaf::rich_edit {
namespace internal {

class ObjectContext {
public:
    ObjectContext(std::size_t position, bool is_in_selection_range) :
        position_(position),
        is_in_selection_range_(is_in_selection_range) {

    }

    std::size_t Position() const {
        return position_;
    }

    bool IsInSelectionRange() const {
        return is_in_selection_range_;
    }

private:
    std::size_t position_{};
    bool is_in_selection_range_{};
};


class ObjectMouseContext : ObjectContext {
public:
    ObjectMouseContext(
        std::size_t position,
        bool is_in_selection_range,
        const Point& mouse_position)
        :
        ObjectContext(position, is_in_selection_range),
        mouse_position_(mouse_position) {

    }

    const Point& MousePosition() const {
        return mouse_position_;
    }

private:
    Point mouse_position_;
};


template<typename E>
class ObjectMouseEventContext : ObjectMouseContext {
public:
    ObjectMouseEventContext(
        std::size_t position,
        bool is_in_selection_range,
        const Point& mouse_position,
        const E& event_info)
        :
        ObjectMouseContext(position, is_in_selection_range, mouse_position),
        event_info_(event_info) {

    }

    const E& EventInfo() const {
        return event_info_;
    }

private:
    E event_info_;
};

}


using PaintContext = internal::ObjectContext;
using MouseCursorContext = internal::ObjectMouseContext;
using MouseDownContext = internal::ObjectMouseEventContext<MouseDownInfo>;
using MouseUpContext = internal::ObjectMouseEventContext<MouseUpInfo>;
using DoubleClickContext = internal::ObjectMouseEventContext<DoubleClickInfo>;

}