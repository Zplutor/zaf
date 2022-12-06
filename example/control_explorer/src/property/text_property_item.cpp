#include "property/text_property_item.h"
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/window/message/keyboard_message.h>

namespace {

class ValueTextBox : public zaf::TextBox {
public:
    typedef std::function<void(ValueTextBox&)> ValueChangeEvent;

public:
    void Initialize() override {

        __super::Initialize();

        SetParagraphAlignment(zaf::ParagraphAlignment::Center);

        Subscriptions() += FocusChangeEvent().Subscribe(std::bind([this]() {

            if (!IsFocused()) {
                if (value_change_event_ != nullptr) {
                    value_change_event_(*this);
                }
            }
        }));
    }

    void RegisterValueChangeEvent(const ValueChangeEvent& event) {
        value_change_event_ = event;
    }

protected:
    bool OnKeyDown(const zaf::KeyMessage& message) override {

        if (__super::OnKeyDown(message)) {
            return true;
        }

        if (message.VirtualKey() == VK_RETURN) {

            SHORT control_key_state = GetKeyState(VK_CONTROL);
            if ((control_key_state >> 15) == 0) {

                if (value_change_event_ != nullptr) {
                    value_change_event_(*this);
                }
                return true;
            }
        }

        return false;
    }

private:
    ValueChangeEvent value_change_event_;
};


std::shared_ptr<ValueTextBox> CreateValueTextBox(
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change, 
    const std::function<void(const std::function<void()>&)>& register_notification) {

    auto text_box = zaf::Create<ValueTextBox>();
    text_box->SetText(get_value());
    text_box->RegisterValueChangeEvent([get_value, value_change](ValueTextBox& text_box) {

        value_change(text_box.Text());
        text_box.SetText(get_value());
    });

    if (register_notification != nullptr) {
        register_notification([text_box, get_value]() {
            text_box->SetText(get_value());
        });
    }

    return text_box;
}


std::shared_ptr<PropertyItem> InnerCreateTextPropertyItem(
    const std::wstring& title,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change, 
    const std::function<void(const std::function<void()>&)>& register_notification,
    bool is_multiline) {

    auto text_box = CreateValueTextBox(get_value, value_change, register_notification);
    text_box->SetIsMultiline(is_multiline);

    return CreatePropertyItem(title, text_box);
}

}


std::shared_ptr<PropertyItem> CreateTextPropertyItem(
    const std::wstring& title,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change, 
    const std::function<void(const std::function<void()>&)>& register_notification) {

    return InnerCreateTextPropertyItem(title, get_value, value_change, register_notification, false);
}

std::shared_ptr<PropertyItem> CreateMultilineTextPropertyItem(
    const std::wstring& title,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change) {

    return InnerCreateTextPropertyItem(title, get_value, value_change, nullptr, true);
}