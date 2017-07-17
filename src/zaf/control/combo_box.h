#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/list_box.h>
#include <zaf/control/text_box.h>

namespace zaf {

namespace internal {
class ComboBoxDropDownWindow;
}

/**
 Represents a combo box control.
 */
class ComboBox : public ClickableControl {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
    /**
     Represents a drop down list box control used in combo box.
     */
    class DropDownListBox : public ListBox {
    public:
        ZAF_DECLARE_TYPE_NAME();

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

    /**
     Represents an edit text box control used in combo box.
     */
    class EditTextBox : public TextBox {
    public:
        ZAF_DECLARE_TYPE_NAME();

    public:
        bool KeyDown(const KeyMessage& message) override;
    };

    /**
     Type of selection change event.
     */
    typedef Event<const std::shared_ptr<ComboBox>&> SelectionChangeEvent;

public:
    ComboBox();
    ~ComboBox();

    void Initialize() override;

    /**
     Get the color picker of drop down button.
     */
    const ColorPicker GetDropDownButtonColorPicker() const;

    /**
     Set the color picker of drop down button.
     */
    void SetDropDownButtonColorPicker(const ColorPicker& color_picker);

    /**
     Get the current color of drop down button.
     */
    const Color GetDropDownButtonColor() const {
        return GetDropDownButtonColorPicker()(*this);
    }

    /**
     Set the color of drop down button.
     */
    void SetDropDownButtonColor(const Color& color) {
        SetDropDownButtonColorPicker([color](const Control&) { return color; });
    }

    /**
     Get the width of drop down button.

     The default width is 12.
     */
    float GetDropDownButtonWidth() const;

    /**
     Set the width of drop down button.
     */
    void SetDropDownButtonWidth(float width);

    /**
     Get the minimum visible item count in drop down list.

     This property affects the minimum height of drop down list. The minimum height
     is equal to the total height of items in specified count.

     The default count is 1.
     */
    std::size_t GetMinimumVisibleItemCount() const;

    /**
     Set the minimum visible item count in drop down list.
     */
    void SetMinimumVisibleItemCount(std::size_t count);

    /**
     Get the maximum visible item count in drop down list.

     This property affects the maximum height of drop down list. The maximum height
     is equal to the total height of items in specified count.

     The default count is the maximum value of std::size_t.
     */
    std::size_t GetMaximumVisibleItemCount() const;

    /**
     Set the maximum visible item count in drop down list.
     */
    void SetMaximumVisibleItemCount(std::size_t count);

    /**
     Get a value indicating that whether the combo box is editable.

     The default value is false.
     */
    bool IsEditable() const;

    /**
     Set a value indicating that whether the combo box is editable.
     */
    void SetIsEditable(bool is_editable);

    /**
     Get selection change event.

     This event is raised when the selection of combo box is changed.
     */
    SelectionChangeEvent::Proxy GetSelectionChangeEvent();

    /**
     Get the drop down list box used in combo box.
     */
    const std::shared_ptr<DropDownListBox>& GetDropDownListBox() const {
        return drop_down_list_box_;
    }

    /**
     Set a new drop down list box to combo box.

     Setting a nullptr would reset the drop down list box to a default one.
     */
    void SetDropDownListBox(const std::shared_ptr<DropDownListBox>& list_box);

    /**
     Get the edit text box used in combo box.
     */
    const std::shared_ptr<EditTextBox>& GetEditTextBox() const {
        return edit_text_box_;
    }

    /**
     Set a new edit text box to combo box.

     Setting a nullptr would reset the edit text box to a default one.
     */
    void SetEditTextBox(const std::shared_ptr<EditTextBox>& text_box);

protected:
    void Layout(const Rect&) override;
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    Rect GetTextRect() override;

    void MouseClick() override;
    bool KeyDown(const KeyMessage& message) override;
    void FocusGain() override;

    /**
     This method is called when the drop down list box is changed.

     @param previous_drop_down_list_box
         The previous drop down list box, may be nullptr.

     Derived classes must call the same method of base class.
     */
    virtual void DropDownListBoxChange(const std::shared_ptr<DropDownListBox>& previous_drop_down_list_box) { }

    /**
     This method is called when the edit text box is changed.

     @param previous_edit_text_box
         The previous edit text box, may be nullptr.

     Derived classes must call the same method of base class.
     */
    virtual void EditTextBoxChange(const std::shared_ptr<TextBox>& previous_edit_text_box) { }

    void DeserializeProperty(const std::wstring& name, const DataNode& data_node) override;

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

    void ConfirmSelection();
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