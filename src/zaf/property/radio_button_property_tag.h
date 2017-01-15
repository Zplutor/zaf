#pragma once

#include <zaf/control/radio_button.h>
#include <zaf/property/clickable_control_property_tag.h>

namespace zaf {

struct IsSelectedPropertySetter{ 
    bool value;                  
    void operator()(RadioButton& object) {  
        if (value) {
            object.SetSelected();              
        }
    }                                  
};      
typedef zaf::internal::PropertyTag<IsSelectedPropertySetter, bool> IsSelectedPropertyTag; 
const IsSelectedPropertyTag isSelected;


ZAF_DEFINE_PROPERTY_TAG(group, std::shared_ptr<RadioButton::Group>, RadioButton, SetGroup);

ZAF_DEFINE_EVENT_PROPERTY_TAG(onSelectStateChange, RadioButton::SelectStateChangeEvent, RadioButton, GetSelectStateChangeEvent);

}