#include <zaf/control/textual/copying_info.h>
#include <zaf/control/text_box.h>

namespace zaf::textual {

CopyingInfo::CopyingInfo(std::shared_ptr<TextBox> source, const Range& selection_range) :
    EventInfo(std::move(source)),
    is_handled_(std::make_shared<bool>()),
    selection_range_(selection_range) {

}

}