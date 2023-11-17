#include <zaf/window/dialog.h>
#include <zaf/base/as.h>
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

    auto owner = Owner();
    if (owner != nullptr) {
        EnableWindow(owner->Handle(), FALSE);
    }

    Show();

    internal::MessageLoop message_loop;
    message_loop.Run();

    if (owner != nullptr) {
        EnableWindow(owner->Handle(), TRUE);
        SetForegroundWindow(owner->Handle());
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

    if (message.ID() == WM_KEYDOWN) {

        if ((message.VirtualKey() == VK_RETURN) || (message.VirtualKey() == VK_ESCAPE)) {

            auto focused_control = FocusedControl();
            if ((focused_control != nullptr) && focused_control->AcceptKeyMessage(message)) {
                return false;
            }

            if (message.VirtualKey() == VK_ESCAPE) {
                CloseWithResult(DialogResult::Cancel);
                return true;
            }
            
            //VK_RETURN
            if (current_default_button_ != nullptr) {
                if (current_default_button_->Window().get() == this) {
                    current_default_button_->Click();
                    return true;
                }
            }
        }
    }

    return __super::PreprocessMessage(message);
}


void Dialog::OnDestroyed(const DestroyedInfo& event_info) {

    __super::OnDestroyed(event_info);

    if (is_showing_modally_) {
        PostQuitMessage(0);
    }
}


void Dialog::OnShow(const ShowInfo& event_info) {

    __super::OnShow(event_info);

    SetCurrentDefaultButton(default_button_);
}


void Dialog::OnFocusedControlChanged(const std::shared_ptr<Control>& previous_focused_control) {

    __super::OnFocusedControlChanged(previous_focused_control);

    auto new_focused_button = std::dynamic_pointer_cast<Button>(FocusedControl());
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
        [this](const ClickInfo& event_info) {

            auto button = As<Button>(event_info.Source());

            if (!RootControl()->IsAncestorOf(*button)) {
                return;
            }

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