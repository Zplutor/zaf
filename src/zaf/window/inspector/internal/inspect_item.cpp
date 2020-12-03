#include <zaf/window/inspector/internal/inspect_item.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {

void InspectItem::Initialize() {

    __super::Initialize();

    SetBorder(1);
    SetBorderColor(Color::Transparent());
}


void InspectItem::MouseEnter(const std::shared_ptr<Control>& entered_control) {

    __super::MouseEnter(entered_control);

    SetBorderColor(Color::FromRGB(ControlSelectedColorRGB));
}


void InspectItem::MouseLeave(const std::shared_ptr<Control>& leaved_control) {

    __super::MouseLeave(leaved_control);

    SetBorderColor(Color::Transparent());
}

}