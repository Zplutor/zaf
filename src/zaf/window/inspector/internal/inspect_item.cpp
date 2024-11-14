#include <zaf/window/inspector/internal/inspect_item.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {

void InspectItem::Initialize() {

    __super::Initialize();

    SetBorder(Frame(1));
    SetBorderColor(Color::Transparent());
}


void InspectItem::OnItemDataChanged() {
    Label()->SetText(GetItemText());
}


std::wstring InspectItem::GetItemText() const {

    auto text = ItemData()->ToString();

    //Erase " class " text.
    [&text]() {

        auto first_space_index = text.find_first_of(L' ');
        if (first_space_index == std::wstring::npos) {
            return;
        }

        auto last_space_index = text.find_last_of(L' ');
        if (last_space_index == std::wstring::npos) {
            return;
        }

        text.erase(first_space_index, last_space_index - first_space_index);
    }();

    //Append control name.
    auto control = dynamic_cast<Control*>(ItemData().get());
    if (control) {

        auto control_name = control->Name();
        if (!control_name.empty()) {
            text += L" - ";
            text += control_name;
        }
    }

    return text;
}


void InspectItem::SetIsHighlight(bool is_highlight) {

    is_highlight_ = is_highlight;

    SetBorderColor(
        is_highlight_ ? Color::FromRGB(ControlSelectedActivedColorRGB) : Color::Transparent());
}

}