#include <zaf/control/property_grid/editable_value_view.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf::property_grid {

void EditableValueView::Initialize() {

    __super::Initialize(); 

    rich_edit_ = Create<RichEdit>();
    rich_edit_->SetBorder(Frame{});
    rich_edit_->SetBackgroundColor(Color::Transparent());
    rich_edit_->SetParagraphAlignment(ParagraphAlignment::Center);
    rich_edit_->SetAllowBeep(false);

    Subscriptions() += rich_edit_->FocusGainedEvent().Subscribe(std::bind([this]() {
        NotifyShouldSelectItem();
    }));

    Subscriptions() += rich_edit_->FocusLostEvent().Subscribe(
        std::bind(&EditableValueView::OnRichEditFocusLost, this, std::placeholders::_1));

    Subscriptions() += rich_edit_->KeyDownEvent().Subscribe(
        [this](const KeyDownInfo& event_info) {
    
        if (event_info.Message().Key() == Key::Enter) {
            ChangeValue();
            event_info.MarkAsHandled();
        }
    });

    this->AddChild(rich_edit_);
}


void EditableValueView::SetAccessMethod(AccessMethod access_method) {

    bool is_read_only = access_method == AccessMethod::ReadOnly;
    rich_edit_->SetIsEnabled(!is_read_only);

    rich_edit_->SetTextColor(Color::FromRGB(
        is_read_only ? 
        zaf::internal::ControlDisabledTextColorRGB : 
        zaf::internal::ControlNormalTextColorRGB));
}


void EditableValueView::SetValue(const std::shared_ptr<Object>& value) {

    value_ = value;
    rich_edit_->SetText(value_->ToString());
}


void EditableValueView::ChangeValue() {

    auto new_text = rich_edit_->Text();
    if (new_text.empty() || new_text == value_->ToString()) {
        return;
    }

    try {

        auto value_type = value_->DynamicType();
        auto new_value = value_type->CreateInstance();
        value_type->Parser()->ParseFromAttribute(new_text, *new_value);

        NotifyValueChanged(new_value);
    }
    catch (const Error&) {
        rich_edit_->SetText(value_->ToString());
    }
}


void EditableValueView::OnRichEditFocusLost(const FocusLostInfo& event_info) {

    //Commit new value only when focus is transferred to other control.
    //Value should not be committed in some situations, such as when window loses focus.
    if (event_info.GainedFocusControl()) {
        ChangeValue();
    }
}


void EditableValueView::RecoverFocus() {
    rich_edit_->SetIsFocused(true);
}

}