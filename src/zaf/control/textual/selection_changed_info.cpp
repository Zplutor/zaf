#include <zaf/control/textual/selection_changed_info.h>
#include <zaf/control/text_box.h>

namespace zaf::textual {

SelectionChangedInfo::SelectionChangedInfo(std::shared_ptr<TextBox> source) : 
    EventInfo(std::move(source)) {

}

}