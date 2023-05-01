#include <zaf/control/event/text_changing_info.h>
#include <zaf/control/rich_edit.h>

namespace zaf {

TextChangingInfo::TextChangingInfo(
    const std::shared_ptr<RichEdit>& source,
    TextChangeReason reason,
    const zaf::TextRange& text_range,
    const Message& triggered_message)
    :
    EventInfo(source),
    reason_(reason),
    text_range_(text_range),
    triggered_message_(triggered_message),
    can_change_(std::make_shared<bool>(true)) {

}

}