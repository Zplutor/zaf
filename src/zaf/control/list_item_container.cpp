#include <zaf/control/list_item_container.h>
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
    SetLayouter(CreateLayouter(std::bind(
        &ListItemContainer::LayoutItems,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    )));
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


bool ListItemContainer::OnMouseDown(const Point& position, const MouseMessage& message) {

    if (__super::OnMouseDown(position, message)) {
        return true;
    }

    SetIsFocused(true);

    if (message.MouseButton() == MouseButton::Left) {
        CaptureMouse();
        select_strategy_->BeginChangingSelectionByMouseDown(position, message);
    }

    return true;
}


bool ListItemContainer::OnMouseMove(const Point& position, const MouseMessage& message) {

    if (__super::OnMouseMove(position, message)) {
        return true;
    }

    if (IsCapturingMouse()) {
        select_strategy_->ChangeSelectionByMouseMove(position, message);
        return true;
    }

    return false;
}


bool ListItemContainer::OnMouseUp(const Point& position, const MouseMessage& message) {

    if (__super::OnMouseUp(position, message)) {
        return true;
    }

    if (message.MouseButton() == MouseButton::Left) {
    
        if (IsCapturingMouse()) {
            ReleaseMouse();
        }

        select_strategy_->EndChangingSelectionByMouseUp(position, message);
    }
    
    return true;
}


bool ListItemContainer::OnKeyDown(const KeyMessage& message) {

    if (__super::OnKeyDown(message)) {
        return true;
    }

    bool is_handled = select_strategy_->ChangeSelectionByKeyDown(message);
    return is_handled;
}

}