#include <zaf/control/label_list_item.h>
#include <zaf/internal/list/list_label.h>

namespace zaf {

ZAF_OBJECT_IMPL(LabelListItem);

void LabelListItem::Initialize() {

    __super::Initialize();

    this->SetPadding(Frame(2, 0, 2, 0));
    InstallLabel(internal::CreateListLabel());
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


void LabelListItem::SetLabel(std::shared_ptr<zaf::Label> label) {

    ZAF_EXPECT(label);

    auto update_guard = BeginUpdate();

    this->RemoveChild(label_);
    InstallLabel(std::move(label));
}


void LabelListItem::InstallLabel(std::shared_ptr<zaf::Label> label) {

    label_ = std::move(label);
    this->AddChild(label_);
}

}