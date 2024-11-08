#include <zaf/control/property_grid/bool_editor.h>
#include <zaf/creation.h>
#include <zaf/object/boxing/boxing.h>

namespace zaf::property_grid {

void BoolEditor::Initialize() {

    __super::Initialize();

    check_box_ = Create<CheckBox>();
    check_box_->SetParagraphAlignment(dwrite::ParagraphAlignment::Center);

    Subscriptions() += check_box_->FocusGainedEvent().Subscribe(std::bind([this]() {
    
        NotifyShouldSelectItem();
    }));

    this->AddChild(check_box_);
}


void BoolEditor::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        check_box_->SetIsEnabled(false);
    }
}


void BoolEditor::SetValue(const std::shared_ptr<Object>& object) {

    check_box_->SetIsChecked(Unbox<bool>(*object));

    Subscriptions() += check_box_->CheckStateChangedEvent().Subscribe(std::bind([this]() {
        NotifyValueChanged(Box(check_box_->IsChecked()));
    }));
}

}