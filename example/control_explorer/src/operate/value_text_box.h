#pragma once 

#include <zaf/control/text_box.h>

class ValueTextBox : public zaf::TextBox {
public:
    typedef std::function<void(ValueTextBox&)> ValueChangeEvent;

public:
    void Initialize() override;

    void RegisterValueChangeEvent(const ValueChangeEvent& event) {
        value_change_event_ = event;
    }

protected:
    bool KeyDown(const zaf::KeyMessage& message) override;

private:
    ValueChangeEvent value_change_event_;
};