#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/window/message/message.h>

namespace zaf {

Dialog::Dialog() : 
    is_showing_modally_(false),
    dialog_result_(DialogResult::None) {

}


Dialog::~Dialog() {

    for (const auto& each_pair : dialog_buttons_) {
        each_pair.first->GetClickEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    }
}


void Dialog::ShowModally() {

    //Just show the window if already shown modally.
    if (is_showing_modally_) {
        Show();
        return;
    }

    is_showing_modally_ = true;
    dialog_result_ = DialogResult::None;

    auto owner = GetOwner();
    if (owner != nullptr) {
        EnableWindow(owner->GetHandle(), FALSE);
    }

    Show();

    MSG msg = { 0 };
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (owner != nullptr) {
        EnableWindow(owner->GetHandle(), TRUE);
        SetForegroundWindow(owner->GetHandle());
    }

    if (dialog_result_ == DialogResult::None) {
        dialog_result_ = DialogResult::Cancel;
    }
    is_showing_modally_ = false;
}


void Dialog::CloseWithResult(DialogResult result) {

    dialog_result_ = result;
    Close();
}


bool Dialog::ReceiveMessage(const Message& message, LRESULT& result) {

    bool return_value = __super::ReceiveMessage(message, result);

    if (message.id == WM_KEYDOWN) {

        if (message.wParam == VK_ESCAPE) {
            CloseWithResult(DialogResult::Cancel);
        }
        else if (message.wParam == VK_RETURN) {

            if (current_default_button_ != nullptr) {
                if (current_default_button_->GetWindow().get() == this) {
                    current_default_button_->Click();
                }
            }
        }
    }

    return return_value;
}


void Dialog::WindowDestroy(HWND handle) {

    __super::WindowDestroy(handle);

    if (is_showing_modally_) {
        PostQuitMessage(0);
    }
}


void Dialog::WindowShow() {

    __super::WindowShow();

    SetCurrentDefaultButton(default_button_);
}


void Dialog::FocusedControlChange(const std::shared_ptr<Control>& previous_focused_control) {

    __super::FocusedControlChange(previous_focused_control);

    auto new_focused_button = std::dynamic_pointer_cast<Button>(GetFocusedControl());
    if (new_focused_button != nullptr) {
        SetCurrentDefaultButton(new_focused_button);
    }
    else {
        SetCurrentDefaultButton(default_button_);
    }
}


void Dialog::AddDialogButton(const std::shared_ptr<Button>& button, DialogResult dialog_result) {

    RemoveDialogButton(button);

    dialog_buttons_.insert(std::make_pair(button, dialog_result));

    button->GetClickEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this), 
        [this](const std::shared_ptr<ClickableControl>& button) {

            if (!GetRootControl()->IsAncestorOf(button)) {
                return;
            }

            auto iterator = dialog_buttons_.find(std::dynamic_pointer_cast<Button>(button));
            if (iterator == dialog_buttons_.end()) {
                return;
            }

            CloseWithResult(iterator->second);
        }
    );
}


bool Dialog::RemoveDialogButton(const std::shared_ptr<Button>& button) {

    auto iterator = dialog_buttons_.find(button);
    if (iterator == dialog_buttons_.end()) {
        return false;
    }

    iterator->first->GetClickEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    dialog_buttons_.erase(iterator);
    return true;
}


void Dialog::SetCurrentDefaultButton(const std::shared_ptr<Button>& button) {

    if (current_default_button_ != nullptr) {
        current_default_button_->SetIsDefault(false);
    }

    current_default_button_ = button;

    if (current_default_button_ != nullptr) {
        current_default_button_->SetIsDefault(true);
    }
}

}