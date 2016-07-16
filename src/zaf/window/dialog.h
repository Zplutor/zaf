#pragma once

#include <map>
#include <zaf/window/dialog_result.h>
#include <zaf/window/window.h>

namespace zaf {

class Button;

class Dialog : public Window {
public:
    Dialog();
    ~Dialog();

    void ShowModally();
    void CloseWithResult(DialogResult result);

    DialogResult GetDialogResult() const {
        return dialog_result_;
    }

    const std::shared_ptr<Button>& GetDefaultButton() const {
        return default_button_;
    }

    void SetDefaultButton(const std::shared_ptr<Button>& button) {
        default_button_ = button;
    }

    void AddDialogButton(const std::shared_ptr<Button>& button, DialogResult dialog_result);
    bool RemoveDialogButton(const std::shared_ptr<Button>& button);

protected:
    bool ReceiveMessage(const Message& message, LRESULT& result) override;
    void WindowDestroy(HWND handle) override;
    void WindowShow() override;
    void FocusedControlChange(const std::shared_ptr<Control>& previous_focused_control) override;

    void SetCurrentDefaultButton(const std::shared_ptr<Button>& button);

private:
    bool is_showing_modally_;
    DialogResult dialog_result_;

    std::shared_ptr<Button> default_button_;
    std::shared_ptr<Button> current_default_button_;
    std::map<std::shared_ptr<Button>, DialogResult> dialog_buttons_;
};

}