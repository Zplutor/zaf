#include <zaf/control/textual/active_inline_object_events.h>
#include <zaf/control/textual/active_inline_object.h>

namespace zaf {
namespace internal {

HandleableInlineObjectEventInfo::HandleableInlineObjectEventInfo(
    std::shared_ptr<textual::ActiveInlineObject> source)
    :
    EventInfo(std::move(source)),
    is_handled_(std::make_shared<bool>()) {

}

}

namespace textual {

InlineObjectMouseEnterInfo::InlineObjectMouseEnterInfo(
    std::shared_ptr<ActiveInlineObject> source) 
    :
    EventInfo(std::move(source)) {

}


InlineObjectMouseLeaveInfo::InlineObjectMouseLeaveInfo(
    std::shared_ptr<ActiveInlineObject> source) 
    :
    EventInfo(std::move(source)) {

}

}
}