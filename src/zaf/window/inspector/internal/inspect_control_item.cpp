#include <zaf/window/inspector/internal/inspect_control_item.h>

namespace zaf::internal {

InspectControlItem::InspectControlItem(const std::shared_ptr<Control>& control) : 
    control_(control) {

}


void InspectControlItem::Initialize() {

    __super::Initialize();

    auto old_color_picker = TextColorPicker();
    SetTextColorPicker([this, old_color_picker](const Control& item) {
    
        if (!control_->IsVisible()) {
            return Color::Gray();
        }

        return old_color_picker(item);
    });
}

}