#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

//Wraps information of WM_SHOWWINDOW message.
class ShowWindowMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    bool IsHidden() const {
        return !WParam();
    }
};

}