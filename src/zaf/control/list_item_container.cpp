#include <zaf/control/list_item_container.h>
#include <zaf/base/as.h>
#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

ListItemContainer::ListItemContainer() {

}


void ListItemContainer::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());
    SetCanFocused(false);
    SetCanDoubleClick(true);
    SetLayouter(CreateLayouter(std::bind_front(&ListItemContainer::LayoutItems, this)));
}


void ListItemContainer::LayoutItems(
    const Control& parent,
    const zaf::Rect& previous_rect,
    const std::vector<std::shared_ptr<Control>>& children) {

    float width = ContentSize().width;

    for (const auto& each_child : children) {

        auto child_rect = each_child->Rect();
        if (child_rect.size.width == width) {
            continue;
        }

        child_rect.size.width = width;
        each_child->SetRect(child_rect);
    }
}


void ListItemContainer::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);

    RepaintSelectedItemsOnFocusChanged(
        As<Control>(event_info.Source()), 
        event_info.LostFocusControl());
}


void ListItemContainer::OnFocusLost(const FocusLostInfo& event_info) {

    __super::OnFocusLost(event_info);

    RepaintSelectedItemsOnFocusChanged(
        As<Control>(event_info.Source()),
        event_info.GainedFocusControl());
}


void ListItemContainer::RepaintSelectedItemsOnFocusChanged(
    const std::shared_ptr<Control>& focus_changed_control,
    const std::shared_ptr<Control>& focus_changing_control) {

    if (AreControlsSelfOrChildren(focus_changed_control, focus_changing_control)) {
        return;
    }

    RepaintSelectedItems();
}


bool ListItemContainer::AreControlsSelfOrChildren(
    const std::shared_ptr<Control>& control1,
    const std::shared_ptr<Control>& control2) const {

    auto is_self_or_child = [this](const std::shared_ptr<Control>& control) {

        if (!control) {
            return false;
        }

        return this->IsSameOrAncestorOf(*control);
    };

    return is_self_or_child(control1) && is_self_or_child(control2);
}


void ListItemContainer::RepaintSelectedItems() {

    for (const auto& each_child : Children()) {

        auto list_item = As<ListItem>(each_child);
        if (!list_item) {
            continue;
        }

        if (list_item->IsSelectedInContext()) {
            list_item->NeedRepaint();
        }
    }
}

}