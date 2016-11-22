#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/list_box.h>
#include <zaf/control/text_box.h>

namespace zaf {

namespace internal {
class ComboBoxDropDownWindow;
}

class ComboBox : public ClickableControl {
public:
    class DropDownListBox : public ListBox {
    public:
        void Initialize() override;

    protected:
        void MouseMove(const Point& position, const MouseMessage& message) override;

    private:
        friend class ComboBox;

        void SetMouseMoveCallback(const std::function<void(const Point&)>& callback) {
            mouse_move_callback_ = callback;
        }

    private:
        std::function<void(const Point&)> mouse_move_callback_;
    };

    class EditTextBox : public TextBox {
    public:
        bool KeyDown(const KeyMessage& message) override;
    };

    typedef Event<const std::shared_ptr<ComboBox>&> SelectionChangeEvent;

public:
    ComboBox();
    ~ComboBox();

    void Initialize() override;

    const ColorPicker GetDropDownButtonColorPicker() const;
    void SetDropDownButtonColorPicker(const ColorPicker& color_picker);

    const Color GetDropDownButtonColor() const {
        return GetDropDownButtonColorPicker()(*this);
    }

    void SetDropDownButtonColor(const Color& color) {
        SetDropDownButtonColorPicker([color](const Control&) { return color; });
    }

    float GetDropDownButtonWidth() const;
    void SetDropDownButtonWidth(float width);

    std::size_t GetMinimumVisibleItemCount() const;
    void SetMinimumVisibleItemCount(std::size_t count);

    std::size_t GetMaximumVisibleItemCount() const;
    void SetMaximumVisibleItemCount(std::size_t count);

    bool IsEditable() const;
    void SetIsEditable(bool is_editable);

    SelectionChangeEvent::Proxy GetSelectionChangeEvent();

    const std::shared_ptr<DropDownListBox>& GetDropDownListBox() const {
        return drop_down_list_box_;
    }

    void SetDropDownListBox(const std::shared_ptr<DropDownListBox>& list_box);

    const std::shared_ptr<EditTextBox>& GetEditTextBox() const {
        return edit_text_box_;
    }

    void SetEditTextBox(const std::shared_ptr<EditTextBox>& text_box);

protected:
    void Layout(const Rect&) override;
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    const Rect GetTextRect() const override;

    void MouseClick() override;
    bool KeyDown(const KeyMessage& message) override;
    void FocusGain() override;

    virtual void DropDownListBoxChange(const std::shared_ptr<DropDownListBox>& previous_drop_down_list_box) { }
    virtual void EditTextBoxChange(const std::shared_ptr<TextBox>& previous_edit_text_box) { }

private:
    enum class SelectionChangeAction {
        CloseDropDownWindow,
        ChangeText,
        Nothing,
    };

    enum class TextChangeAction {
        UnselectDropDownItem,
        Nothing,
    };

    template<typename ActionType>
    class ActionGuard {
    public:
        ActionGuard(ActionType* action, ActionType default_action) : 
            action_(action),
            default_action_(default_action) {
        
        }

        ActionGuard(ActionGuard&& rhs) :
            action_(rhs.action_),
            default_action_(rhs.default_action_) {
            rhs.action_ = nullptr;
        }

        ~ActionGuard() {
            if (action_ != nullptr) {
                *action_ = default_action_;
            }
        }

        ActionGuard(const ActionGuard&) = delete;
        ActionGuard& operator=(const ActionGuard&) = delete;

    private:
        ActionType* action_;
        ActionType default_action_;
    };

private:
    void InitializeDropDownListBox();
    void UninitializeDropDownListBox();

    void InitializeEditTextBox();
    void UninitializeEditTextBox();

    void PopupDropDownWindow();
    float CalculateDropDownListHeight(std::size_t visible_item_count);

    void DropDownListBoxMouseMove(const Point& position);
    bool SelectNextDropDownListItem(bool reverse);

    void DropDownListBoxSelectionChange();
    void NotifySelectionChange();

    void EnterKeyDown();
    void EditTextBoxTextChange();

    ActionGuard<SelectionChangeAction> SetSelectionChangeAction(SelectionChangeAction action);
    ActionGuard<TextChangeAction> SetTextChangeAction(TextChangeAction action);

private:
    std::shared_ptr<EditTextBox> edit_text_box_;
    std::shared_ptr<internal::ComboBoxDropDownWindow> drop_down_window_;
    std::shared_ptr<DropDownListBox> drop_down_list_box_;
    SelectionChangeAction selection_change_action_;
    TextChangeAction text_change_action_;
};

}