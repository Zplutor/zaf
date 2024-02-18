#include <zaf/control/rich_edit/rich_edit_events.h>
#include <zaf/control/rich_edit.h>

namespace zaf::rich_edit {

TextChangingInfo::TextChangingInfo(
    std::shared_ptr<RichEdit> source,
    TextChangeReason reason,
    const Range& text_range,
    const Message& triggered_message)
    :
    EventInfo(std::move(source)),
    reason_(reason),
    text_range_(text_range),
    triggered_message_(triggered_message),
    can_change_(std::make_shared<bool>(true)) {

}


TextChangedInfo::TextChangedInfo(std::shared_ptr<RichEdit> source) : EventInfo(std::move(source)) {

}


SelectionChangedInfo::SelectionChangedInfo(std::shared_ptr<RichEdit> source) : 
    EventInfo(std::move(source)) {

}

}