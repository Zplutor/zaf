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
    expand_button_->GetClickEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
        std::bind(&TreeItem::ExpandButtonClick, this));

    AddChild(expand_button_);
}


void TreeItem::Layout(const Rect& previous_rect) {

    __super::Layout(previous_rect);

    expand_button_->SetRect(GetExpandButtonRect());
}


Rect TreeItem::GetTextRect() {

    float left = (indent_deep_ + 1) * ExpandButtonSize + ExpandButtonMargin;

    auto rect = __super::GetTextRect();
    rect.Deflate(Frame{ left, 0, 0, 0 });
    return rect;
}


Rect TreeItem::GetExpandButtonRect() {

    Rect result;
    result.size.width = ExpandButtonSize;
    result.size.height = ExpandButtonSize;
    result.position.x = indent_deep_ * ExpandButtonSize;
    result.position.y = (GetContentSize().height - ExpandButtonSize) / 2;
    return result;
}


void TreeItem::SetExpandState(ExpandState expand_state) {
    expand_button_->SetExpandState(expand_state);
}


void TreeItem::SetIndentDeep(std::size_t deep) {

    indent_deep_ = deep;
    NeedRelayout();
}


void TreeItem::ExpandButtonClick() {

    auto tree_control_implementation = tree_control_implementation_.lock();
    if (!tree_control_implementation) {
        return;
    }

    bool previous_is_expand = expand_button_->GetExpandState() == ExpandState::Expanded;

    tree_control_implementation->OnItemExpandChange(
        std::dynamic_pointer_cast<TreeItem>(shared_from_this()),
        !previous_is_expand);
}

}