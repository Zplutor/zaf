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

    void AddDialogButton(const std::shared_ptr<Button>& button, DialogResult dialog_result);

    DialogResult GetDialogResult() const {
        return dialog_result_;
    }

protected:
    bool ReceiveMessage(const Message& message, LRESULT& result) override;
    void WindowDestroy(HWND handle) override;

private:
    bool is_showing_modally_;
    DialogResult dialog_result_;

    std::map<std::shared_ptr<Button>, DialogResult> dialog_buttons_;
};

}