#include <zaf/control/textual/inline_object_events.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf::textual {

MouseCursorChangingInfo::MouseCursorChangingInfo(std::shared_ptr<InlineObject> source) :
    EventInfo(std::move(source)) {

}

}