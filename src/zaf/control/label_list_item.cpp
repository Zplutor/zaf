#include <zaf/control/label_list_item.h>
#include <zaf/internal/list/list_label.h>

namespace zaf {

ZAF_OBJECT_IMPL(LabelListItem);

void LabelListItem::Initialize() {

    __super::Initialize();

    label_ = internal::CreateListLabel();
    this->AddChild(label_);
}


void LabelListItem::Layout(const zaf::Rect& old_rect) {

    zaf::Rect label_rect{ {}, ContentSize() };
    label_->SetRect(label_rect);
}


void LabelListItem::OnItemDataChanged() {
    label_->SetText(ItemData()->ToString());
}


const std::shared_ptr<Label>& LabelListItem::Label() const noexcept {
    return label_;
}

}