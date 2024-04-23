#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

class IMECompositionMessage : public MessageShim {
public:
    using MessageShim::MessageShim;
};

}