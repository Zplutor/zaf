#include <zaf/control/property_grid/editable_value_view.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf::property_grid {

void EditableValueView::Initialize() {

    __super::Initialize(); 

    text_box_ = Create<TextBox>();
    text_box_->SetBorder(Frame{});
    text_box_->SetBackgroundColor(Color::Transparent());
    text_box_->SetParagraphAlignment(ParagraphAlignment::Center);
    text_box_->SetAllowBeep(false);

    Subscriptions() += text_box_->FocusChangeEvent().Subscribe(std::bind([this]() {

        if (text_box_->IsFocused()) {
            NotifyShouldSelectItem();
        }
        else {
            ChangeValue();
        }
    }));

    this->AddChild(text_box_);
}


void EditableValueView::SetAccessMethod(AccessMethod access_method) {

    bool is_read_only = access_method == AccessMethod::ReadOnly;
    text_box_->SetIsEnabled(!is_read_only);

    text_box_->SetTextColor(Color::FromRGB(
        is_read_only ? 
        zaf::internal::ControlDisabledTextColorRGB : 
        zaf::internal::ControlNormalTextColorRGB));
}


void EditableValueView::SetValue(const std::shared_ptr<Object>& value) {

    value_ = value;
    text_box_->SetText(value_->ToString());
}


bool EditableValueView::OnKeyDown(const KeyMessage& message) {

    if (message.VirtualKey() == VK_RETURN) {
        ChangeValue();
        return true;
    }
    
    return __super::OnKeyDown(message);
}


void EditableValueView::ChangeValue() {

    auto new_text = text_box_->Text();
    if (new_text.empty() || new_text == value_->ToString()) {
        return;
    }

    try {

        auto value_type = value_->GetType();
        auto new_value = value_type->CreateInstance();
        value_type->GetParser()->ParseFromAttribute(new_text, *new_value);

        NotifyValueChanged(new_value);
    }
    catch (const Error&) {
        text_box_->SetText(value_->ToString());
    }
}

}