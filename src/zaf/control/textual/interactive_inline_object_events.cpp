#include <zaf/control/textual/interactive_inline_object_events.h>
#include <zaf/control/textual/interactive_inline_object.h>

namespace zaf {
namespace internal {

HandleableInlineObjectEventInfo::HandleableInlineObjectEventInfo(
    std::shared_ptr<textual::InteractiveInlineObject> source)
    :
    EventInfo(std::move(source)),
    is_handled_(std::make_shared<bool>()) {

}

}

namespace textual {

MouseEnterInfo::MouseEnterInfo(std::shared_ptr<InteractiveInlineObject> source) :
    EventInfo(std::move(source)) {

}


MouseLeaveInfo::MouseLeaveInfo(std::shared_ptr<InteractiveInlineObject> source) :
    EventInfo(std::move(source)) {

}

}
}