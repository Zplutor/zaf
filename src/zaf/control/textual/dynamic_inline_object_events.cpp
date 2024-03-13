#include <zaf/control/textual/dynamic_inline_object_events.h>
#include <zaf/control/textual/dynamic_inline_object.h>

namespace zaf::textual {

MouseEnterInfo::MouseEnterInfo(std::shared_ptr<DynamicInlineObject> source) :
    EventInfo(std::move(source)) {

}


MouseLeaveInfo::MouseLeaveInfo(std::shared_ptr<DynamicInlineObject> source) : 
    EventInfo(std::move(source)) {

}


HandleableEventInfo::HandleableEventInfo(std::shared_ptr<DynamicInlineObject> source) : 
    EventInfo(std::move(source)),
    is_handled_(std::make_shared<bool>()) {

}


MouseUpInfo::MouseUpInfo(std::shared_ptr<DynamicInlineObject> source) : 
    EventInfo(std::move(source)) {

}

}