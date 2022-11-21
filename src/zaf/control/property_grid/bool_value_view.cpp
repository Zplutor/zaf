#include <zaf/control/property_grid/bool_value_view.h>
#include <zaf/creation.h>
#include <zaf/object/boxing/boxing.h>

namespace zaf::property_grid {

void BoolValueView::Initialize() {

    __super::Initialize();

    check_box_ = Create<CheckBox>();
    check_box_->SetParagraphAlignment(ParagraphAlignment::Center);
    this->AddChild(check_box_);
}


void BoolValueView::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        check_box_->SetIsEnabled(false);
    }
}


void BoolValueView::SetValue(const std::shared_ptr<Object>& object) {

    check_box_->SetIsChecked(Unbox<bool>(*object));

    Subscriptions() += check_box_->CheckStateChangeEvent().Subscribe(std::bind([this]() {
        NotifyValueChanged(Box(check_box_->IsChecked()));
    }));
}

}