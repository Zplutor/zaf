#pragma once

#include <zaf/window/message/message.h>

namespace zaf {

class MessageShim {
public:
    explicit MessageShim(const Message& inner) : inner_(inner) { }
    virtual ~MessageShim() = default;

    MessageShim(const MessageShim&) = delete;
    MessageShim& operator=(const MessageShim&) = delete;

    MessageShim(MessageShim&&) = default;
    MessageShim& operator=(MessageShim&&) = default;

    HWND WindowHandle() const {
        return inner_.hwnd;
    }

    UINT ID() const {
        return inner_.id;
    }

    WPARAM WParam() const {
        return inner_.wparam;
    }

    LPARAM LParam() const {
        return inner_.lparam;
    }

    const Message& Inner() const {
        return inner_;
    }

private:
    const Message& inner_;
};

}