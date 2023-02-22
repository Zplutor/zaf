#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/window/message/message.h>

namespace zaf {

class MessageShim : NonCopyable {
public:
    explicit MessageShim(const Message& inner) : inner_(inner) { }
    virtual ~MessageShim() = default;

    MessageShim(MessageShim&&) = default;
    MessageShim& operator=(MessageShim&&) = default;

    HWND WindowHandle() const {
        return inner_.WindowHandle();
    }

    UINT ID() const {
        return inner_.ID();
    }

    WPARAM WParam() const {
        return inner_.WParam();
    }

    LPARAM LParam() const {
        return inner_.LParam();
    }

    const Message& Inner() const {
        return inner_;
    }

private:
    const Message& inner_;
};

}