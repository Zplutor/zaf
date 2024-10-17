#include <zaf/control/list_item_container.h>
#include <zaf/base/as.h>
#include <zaf/control/internal/list_control/list_control_single_select_strategy.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

ListItemContainer::ListItemContainer() {

}


void ListItemContainer::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());
    SetCanFocused(true);
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


void ListItemContainer::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    SetIsFocused(true);

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        CaptureMouse();

        select_strategy_->BeginChangingSelectionByMouseDown(
            event_info.PositionAtSender(), 
            event_info.Message());
    }

    event_info.MarkAsHandled();
}


void ListItemContainer::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (IsCapturingMouse()) {

        select_strategy_->ChangeSelectionByMouseMove(
            event_info.PositionAtSender(),
            event_info.Message());

        event_info.MarkAsHandled();
    }
}


void ListItemContainer::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {
    
        if (IsCapturingMouse()) {
            ReleaseMouse();
        }

        select_strategy_->EndChangingSelectionByMouseUp(
            event_info.PositionAtSender(), 
            event_info.Message());

        event_info.MarkAsHandled();
    }
}


void ListItemContainer::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    bool is_handled = select_strategy_->ChangeSelectionByKeyDown(event_info.Message());
    if (is_handled) {
        event_info.MarkAsHandled();
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