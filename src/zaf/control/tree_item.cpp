#include <zaf/control/tree_item.h>
#include <zaf/control/button.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/tree_item_expand_button.h>
#include <zaf/creation.h>

namespace zaf {
namespace {

constexpr float ExpandButtonSize = 16;
constexpr float ExpandButtonMargin = 2;

}

void TreeItem::Initialize() {

    __super::Initialize();

    expand_button_ = Create<TreeItemExpandButton>();

    Subscriptions() += expand_button_->ClickEvent().Subscribe(
        std::bind(&TreeItem::ExpandButtonClick, this));

    AddChild(expand_button_);
}


void TreeItem::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    expand_button_->SetRect(GetExpandButtonRect());
}


zaf::Rect TreeItem::DetermineTextRect() {

    float left = (indent_level_ + 1) * ExpandButtonSize + ExpandButtonMargin;

    zaf::Rect rect;
    rect.size = this->ContentSize();
    rect.Deflate(Frame{ left, 0, 0, 0 });
    return rect;
}


zaf::Rect TreeItem::GetExpandButtonRect() {

    zaf::Rect result;
    result.size.width = ExpandButtonSize;
    result.size.height = ExpandButtonSize;
    result.position.x = indent_level_ * ExpandButtonSize;
    result.position.y = (ContentSize().height - ExpandButtonSize) / 2;
    return result;
}


ExpandState TreeItem::ExpandState() const {
    return expand_button_->GetExpandState();
}


void TreeItem::SetExpandState(zaf::ExpandState expand_state) {
    expand_button_->SetExpandState(expand_state);
}


void TreeItem::SetIndentLevel(std::size_t deep) {

    indent_level_ = deep;
    NeedRelayout();
}


void TreeItem::ExpandButtonClick() {

    auto tree_control_implementation = tree_control_implementation_.lock();
    if (!tree_control_implementation) {
        return;
    }

    bool previous_is_expand = expand_button_->GetExpandState() == ExpandState::Expanded;

    bool change_succeeded = tree_control_implementation->ChangeItemExpandState(
        std::dynamic_pointer_cast<TreeItem>(shared_from_this()),
        !previous_is_expand);

    if (change_succeeded) {
        expand_button_->SetExpandState(
            previous_is_expand ? ExpandState::Collapsed : ExpandState::Expanded);
    }
}

}