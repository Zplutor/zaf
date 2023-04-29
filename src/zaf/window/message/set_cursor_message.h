#pragma once

#include <zaf/window/message/hit_test_result.h>
#include <zaf/window/message/message_shim.h>

namespace zaf {

/**
Wraps information of a WM_SETCURSOR message.
*/
class SetCursorMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    zaf::HitTestResult HitTestResult() const {
        return static_cast<zaf::HitTestResult>(LOWORD(LParam()));
    }

    UINT TriggeredMessageID() const {
        return HIWORD(LParam());
    }
};

}