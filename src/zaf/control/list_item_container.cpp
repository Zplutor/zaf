#include <zaf/control/list_item_container.h>

namespace zaf {

void ListItemContainer::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());
    SetCanFocus(false);
    SetCanDoubleClick(true);
}


void ListItemContainer::Layout(const zaf::Rect& previous_rect) {

    float width = ContentSize().width;

    for (const auto& each_child : Children()) {

        auto child_rect = each_child->Rect();
        if (child_rect.size.width == width) {
            continue;
        }

        child_rect.size.width = width;
        each_child->SetRect(child_rect);
    }
}

}