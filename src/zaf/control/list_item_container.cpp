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
    const Rect& previous_rect,
    const std::vector<std::shared_ptr<Control>>& children) {

    float width = GetContentSize().width;

    for (const auto& each_child : children) {

        auto child_rect = each_child->GetRect();
        if (child_rect.size.width == width) {
            continue;
        }

        child_rect.size.width = width;
        each_child->SetRect(child_rect);
    }
}


bool ListItemContainer::MouseDown(const Point& position, const MouseMessage& message) {

    SetIsFocused(true);

    if (message.GetMouseButton() != MouseButton::Left) {
        return __super::MouseDown(position, message);
    }

    CaptureMouse();
    select_strategy_->BeginChangingSelectionByMouseDown(position, message);
    return true;
}


bool ListItemContainer::MouseMove(const Point& position, const MouseMessage& message) {

    if (IsCapturingMouse()) {
        select_strategy_->ChangeSelectionByMouseMove(position, message);
        return true;
    }
    else {
        return __super::MouseMove(position, message);
    }
}


bool ListItemContainer::MouseUp(const Point& position, const MouseMessage& message) {

    if (message.GetMouseButton() != MouseButton::Left) {
        return __super::MouseUp(position, message);
    }

    if (IsCapturingMouse()) {
        ReleaseMouse();
    }

    select_strategy_->EndChangingSelectionByMouseUp(position, message);
    return true;
}


bool ListItemContainer::KeyDown(const KeyMessage& message) {

    bool is_handled = select_strategy_->ChangeSelectionByKeyDown(message);
    if (is_handled) {
        return true;
    }
    else {
        return __super::KeyDown(message);
    }
}

}