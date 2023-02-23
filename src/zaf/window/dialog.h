#pragma once

#include <map>
#include <zaf/window/dialog_result.h>
#include <zaf/window/window.h>

namespace zaf {

class Button;

/**
 Represents a dialog box window. 
 */
class Dialog : public Window {
public:
    /**
     Construct the instance.
     */
    Dialog();

    /**
     Destruct the instance.
     */
    ~Dialog();

    /**
     Show the dialog box modally.
     */
    void ShowModally();

    /**
     Clsoe the dialog box with specified result.
     */
    void CloseWithResult(DialogResult result);

    /**
     Get the result of the dialog box.

     If the dialog box is not closed yet, the dialog result is None.
     If the dialog box is closed with pressing ESC or clicking the close button,
     the dialog result is Cancel.

     You can use AddDialogButton method to specify which result would be returned when 
     clicking the button.
     */
    DialogResult GetDialogResult() const {
        return dialog_result_;
    }

    /**
     Get the default button in the dialog box.
     */
    const std::shared_ptr<Button>& GetDefaultButton() const {
        return default_button_;
    }

    /**
     Set the default button in the dialog box.

     @param button
        The default button te be set. Can be nullptr, or it must be a control in the dialog box.

     The default button is clicked when pressing the ENTER key. It can be nullptr, means no default button.

     The default button of a dialog box is not fixed, it would be changed to the button that has focus.
     However, it would be set back to the specified one when no other button has focus.
     */
    void SetDefaultButton(const std::shared_ptr<Button>& button) {
        default_button_ = button;
    }

    /**
     Add a dialog button to the dialog box.

     @param button
        The dialog button to be added. It must be a control in the dialog box.

     @param dialog_result
        The result value associated to the dialog button. 

     A dialog button associates with a DialogResult value. When the button clicked, the dialog box 
     would close and its result would set to the specified value.
     */
    void AddDialogButton(const std::shared_ptr<Button>& button, DialogResult dialog_result);

    /**
     Remove the specified dialog button in the dialog box.

     @return
        Return true if the button is removed; otherwise return false.
     */
    bool RemoveDialogButton(const std::shared_ptr<Button>& button);

protected:
    bool PreprocessMessage(const KeyMessage& message) override;
    void OnDestroyed(const DestroyedInfo& event_info) override;
    void OnShow(const ShowInfo& event_info) override;
    void OnFocusedControlChanged(const std::shared_ptr<Control>& previous_focused_control) override;

private:
    class DialogButtonItem {
    public:
        DialogResult dialog_result{ DialogResult::None };
        Subscription click_event_subscription;
    };

private:
    void SetCurrentDefaultButton(const std::shared_ptr<Button>& button);

private:
    bool is_showing_modally_;
    DialogResult dialog_result_;

    std::shared_ptr<Button> default_button_;
    std::shared_ptr<Button> current_default_button_;
    std::map<std::shared_ptr<Button>, DialogButtonItem> dialog_buttons_;
};

}