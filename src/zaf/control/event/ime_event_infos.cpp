#include <zaf/control/event/ime_event_infos.h>
#include <zaf/control/control.h>

namespace zaf {

IMEStartCompositionInfo::IMEStartCompositionInfo(
    std::shared_ptr<Control> source,
    const zaf::Message& message) 
    :
    EventInfo(std::move(source)),
    message_(message) {

}


IMECompositionInfo::IMECompositionInfo(
    std::shared_ptr<Control> source, 
    const zaf::Message& message)
    : 
    EventInfo(std::move(source)),
    message_(message) {

}


IMEEndCompositionInfo::IMEEndCompositionInfo(
    std::shared_ptr<Control> source,
    const zaf::Message& message)
    :
    EventInfo(std::move(source)),
    message_(message) {

}

}