#include "operate/value_text_box.h"
#include <zaf/window/message/keyboard_message.h>

void ValueTextBox::Initialize() {

    __super::Initialize();

    SetParagraphAlignment(zaf::ParagraphAlignment::Center);

    GetFocusChangeEvent().AddListener(std::bind([this]() {

        if (! IsFocused()) {
            if (value_change_event_ != nullptr) {
                value_change_event_(*this);
            }
        }
    }));
}


bool ValueTextBox::KeyDown(const zaf::KeyMessage& message) {

    if (message.GetVirtualKey() == VK_RETURN) {

        SHORT control_key_state = GetKeyState(VK_CONTROL);
        if ((control_key_state >> 15) == 0) {

            if (value_change_event_ != nullptr) {
                value_change_event_(*this);
            }
            return true;
        }
    }

    return __super::KeyDown(message);
}