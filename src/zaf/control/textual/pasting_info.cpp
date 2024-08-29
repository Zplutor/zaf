#include <zaf/control/textual/pasting_info.h>
#include <zaf/control/text_box.h>

namespace zaf::textual {

PastingInfo::PastingInfo(
    std::shared_ptr<TextBox> source,
    clipboard::DataObject data_object)
    :
    EventInfo(std::move(source)),
    data_object_(std::move(data_object)),
    is_handled_(std::make_shared<bool>()) {

}

}