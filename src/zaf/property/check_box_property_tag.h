#pragma once

#include <zaf/control/check_box.h>
#include <zaf/property/property_tag.h>

namespace zaf {

ZAF_DEFINE_PROPERTY_TAG(canAutoChangeCheckState, bool, CheckBox, SetCanAutoChangeCheckState);
ZAF_DEFINE_PROPERTY_TAG(canBeIndeterminate, bool, CheckBox, SetCanBeIndeterminate);
ZAF_DEFINE_PROPERTY_TAG(checkState, CheckState, CheckBox, SetCheckState);
ZAF_DEFINE_PROPERTY_TAG(isChecked, bool, CheckBox, SetIsChecked);

ZAF_DEFINE_EVENT_PROPERTY_TAG(onCheckStateChange, CheckBox::CheckStateChangeEvent, CheckBox, GetCheckStateChangeEvent);

}