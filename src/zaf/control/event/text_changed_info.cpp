#include <zaf/control/event/text_changed_info.h>
#include <zaf/control/textual_control.h>

namespace zaf {

TextChangedInfo::TextChangedInfo(const std::shared_ptr<TextualControl>& source) : 
    EventInfo(source) {

}

}