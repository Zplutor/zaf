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
        each_pair.first->GetClickEvent().RemoveListeners(this);
    }
}


bool Dialog::ReceiveMessage(const Message& message, LRESULT& result) {

    bool return_value = __super::ReceiveMessage(message, result);

    if (message.id == WM_KEYDOWN) {

        if (message.wParam == VK_ESCAPE) {
            CloseWithResult(DialogResult::Cancel);
        }
        else if (message.wParam == VK_RETURN) {

            for (const auto& each_pair : dialog_buttons_) {
                if (each_pair.first->IsDefault()) {
                    CloseWithResult(each_pair.second);
                    break;
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


void Dialog::ShowModally() {

    //Just show the window if already shown modally.
    if (is_showing_modally_) {
        Show();
        return;
    }

    is_showing_modally_ = true;

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

    is_showing_modally_ = false;
}


void Dialog::CloseWithResult(DialogResult result) {

    dialog_result_ = result;
    Close();
}


void Dialog::AddDialogButton(const std::shared_ptr<Button>& button, DialogResult dialog_result) {

    dialog_buttons_[button] = dialog_result;
    button->GetClickEvent().RemoveListeners(this);

    auto dialog_button_click_callback = [this](const std::shared_ptr<ClickableControl>& button) {

        if (! GetRootControl()->IsAncestorOf(button)) {
            return;
        }

        auto iterator = dialog_buttons_.find(std::dynamic_pointer_cast<Button>(button));
        if (iterator == dialog_buttons_.end()) {
            return;
        }

        CloseWithResult(iterator->second);
    };

    button->GetClickEvent().AddListener(dialog_button_click_callback, this);
}

}