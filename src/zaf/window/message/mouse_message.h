#pragma once

#include <zaf/graphic/point.h>
#include <zaf/window/message/message.h>

namespace zaf {

enum class MouseButton {
    Unspecified,
    Left,
    Middle,
    Right,
};


class MouseMessage : public Message {
public:
    MouseMessage() : button(MouseButton::Unspecified) { }

public:
    Point position;
    MouseButton button;
};


class MouseWheelMessage : public MouseMessage {
public:
    MouseWheelMessage() : is_horizontal(false), distance(0) { }

public:
    bool is_horizontal;
    int distance;
};

}