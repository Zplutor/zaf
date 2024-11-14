#include <zaf/control/label_tree_item.h>
#include <zaf/internal/list/list_label.h>

namespace zaf {

void LabelTreeItem::Initialize() {

    __super::Initialize();

    label_ = internal::CreateListLabel();
    this->AddChild(label_);
}


void LabelTreeItem::Layout(const zaf::Rect& old_rect) {

    __super::Layout(old_rect);

    label_->SetRect(BodyRect());
}


void LabelTreeItem::OnItemDataChanged() {
    label_->SetText(ItemData()->ToString());
}


const std::shared_ptr<zaf::Label>& LabelTreeItem::Label() const noexcept {
    return label_;
}

}