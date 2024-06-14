#include <zaf/window/inspector/internal/inspect_control_item.h>

namespace zaf::internal {

InspectControlItem::InspectControlItem(const std::shared_ptr<Control>& control) : 
    control_(control) {

}


void InspectControlItem::UpdateStyle() {

    __super::UpdateStyle();

    SetTextColor([this]() {

        if (this->IsSelectedInContext()) {
            return Color::White();
        }

        return control_->IsVisibleInContext() ? Color::Black() : Color::Gray();
    }());
}

}