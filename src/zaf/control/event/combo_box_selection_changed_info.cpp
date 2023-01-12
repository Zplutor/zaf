#include <zaf/control/event/combo_box_selection_changed_info.h>
#include <zaf/control/combo_box.h>

namespace zaf {

ComboBoxSelectionChangedInfo::ComboBoxSelectionChangedInfo(
    const std::shared_ptr<ComboBox>& source) 
    :
    EventInfo(source) {

}

}