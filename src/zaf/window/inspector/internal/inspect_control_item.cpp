#include <zaf/window/inspector/internal/inspect_control_item.h>

namespace zaf::internal {

InspectControlItem::InspectControlItem(const std::shared_ptr<Control>& control) : 
    control_(control) {

}


void InspectControlItem::Initialize() {

    __super::Initialize();

    SetTextColor([this]() {

        if (IsSelectedInContext()) {
            return Color::White();
        }

        return control_->IsVisibleInContext() ? Color::Black() : Color::Gray();
    }());
}

}