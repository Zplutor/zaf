#include <zaf/control/event/spin_box_value_changed_info.h>
#include <zaf/control/spin_box.h>

namespace zaf {

SpinBoxValueChangedInfo::SpinBoxValueChangedInfo(
    const std::shared_ptr<SpinBox>& source, 
    int previous_value)
    :
    EventInfo(source),
    previous_value_(previous_value) {

}

}