#include <zaf/control/tree_item.h>
#include <zaf/control/button.h>
#include <zaf/internal/tree/tree_core.h>
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

    expand_button_->SetRect(ExpanderRect());
}


zaf::Rect TreeItem::ExpanderRect() const noexcept {

    zaf::Rect result;
    result.size.width = ExpandButtonSize;
    result.size.height = ExpandButtonSize;
    result.position.x = indent_level_ * ExpandButtonSize;
    result.position.y = (ContentSize().height - ExpandButtonSize) / 2;
    return result;
}


zaf::Rect TreeItem::BodyRect() const noexcept {

    float left = (indent_level_ + 1) * ExpandButtonSize + ExpandButtonMargin;

    zaf::Rect rect;
    rect.size = this->ContentSize();
    rect.Deflate(Frame{ left, 0, 0, 0 });
    return rect;
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

    auto tree_core = tree_core_.lock();
    if (!tree_core) {
        return;
    }

    bool previous_is_expand = expand_button_->GetExpandState() == ExpandState::Expanded;

    bool change_succeeded = tree_core->ChangeItemExpandState(
        std::dynamic_pointer_cast<TreeItem>(shared_from_this()),
        !previous_is_expand);

    if (change_succeeded) {
        expand_button_->SetExpandState(
            previous_is_expand ? ExpandState::Collapsed : ExpandState::Expanded);
    }
}

}