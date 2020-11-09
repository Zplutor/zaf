#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/internal/message_loop.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {

Dialog::Dialog() : 
    is_showing_modally_(false),
    dialog_result_(DialogResult::None) {

}


Dialog::~Dialog() {

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

    internal::MessageLoop message_loop;
    message_loop.Run();

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


bool Dialog::PreprocessMessage(const KeyMessage& message) {

    if (message.id == WM_KEYDOWN) {

        if ((message.GetVirtualKey() == VK_RETURN) || (message.GetVirtualKey() == VK_ESCAPE)) {

            auto focused_control = GetFocusedControl();
            if ((focused_control != nullptr) && focused_control->AcceptKeyMessage(message)) {
                return false;
            }

            if (message.GetVirtualKey() == VK_ESCAPE) {
                CloseWithResult(DialogResult::Cancel);
                return true;
            }
            
            //VK_RETURN
            if (current_default_button_ != nullptr) {
                if (current_default_button_->GetWindow().get() == this) {
                    current_default_button_->Click();
                    return true;
                }
            }
        }
    }

    return __super::PreprocessMessage(message);
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

    DialogButtonItem item;
    item.dialog_result = dialog_result;
    item.click_event_subscription = button->ClickEvent().Subscribe(
        [this](const ClickableControlClickInfo& event_info) {

            if (!GetRootControl()->IsAncestorOf(event_info.clickable_control)) {
                return;
            }

            auto button = std::dynamic_pointer_cast<Button>(event_info.clickable_control);
            auto iterator = dialog_buttons_.find(button);
            if (iterator == dialog_buttons_.end()) {
                return;
            }

            CloseWithResult(iterator->second.dialog_result);
        }
    );

    dialog_buttons_[button] = std::move(item);
}


bool Dialog::RemoveDialogButton(const std::shared_ptr<Button>& button) {

    auto iterator = dialog_buttons_.find(button);
    if (iterator == dialog_buttons_.end()) {
        return false;
    }

    iterator->second.click_event_subscription.Unsubscribe();

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