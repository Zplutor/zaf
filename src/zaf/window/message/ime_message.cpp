#include <zaf/window/message/ime_message.h>

namespace zaf {

bool IMECompositionMessage::HasResultString() const noexcept {
    return (LParam()& GCS_RESULTSTR) != 0;
}


bool IMECompositionMessage::HasCompositionString() const noexcept {
    return (LParam() & GCS_COMPSTR) != 0;
}

}