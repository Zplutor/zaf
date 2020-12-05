#include <zaf/window/inspector/internal/inspect_item.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {

void InspectItem::Initialize() {

    __super::Initialize();

    SetBorder(1);
    SetBorderColor(Color::Transparent());
}


void InspectItem::SetIsHighlight(bool is_highlight) {

    is_highlight_ = is_highlight;

    SetBorderColor(is_highlight_ ? Color::FromRGB(ControlSelectedColorRGB) : Color::Transparent());
}

}