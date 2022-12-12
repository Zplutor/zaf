#include <zaf/window/inspector/internal/inspect_control_item.h>

namespace zaf::internal {

InspectControlItem::InspectControlItem(const std::shared_ptr<Control>& control) : 
    control_(control) {

}


void InspectControlItem::Initialize() {

    __super::Initialize();

    SetTextColorPicker([this](const Control& item) {
    
        if (this->IsSelected()) {
            return Color::White();
        }

        return control_->IsVisible() ? Color::Black() : Color::Gray();
    });
}

}