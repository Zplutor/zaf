#include <zaf/control/textual/dynamic_inline_object_events.h>
#include <zaf/control/textual/dynamic_inline_object.h>

namespace zaf::textual {

MouseCursorChangingInfo::MouseCursorChangingInfo(std::shared_ptr<DynamicInlineObject> source) : 
    EventInfo(std::move(source)) {

}

}