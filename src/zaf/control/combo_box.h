#pragma once

#include <optional>
#include <zaf/control/clickable_control.h>
#include <zaf/control/list_box.h>
#include <zaf/control/text_box.h>
#include <zaf/rx/observable.h>

namespace zaf {
namespace internal {
class ComboBoxDropDownWindow;
}

class ComboBoxDropDownListBox;
class ComboBoxEditTextBox;
class ComboBoxSelectionChangeInfo;

/**
 Represents a combo box control.
 */
class ComboBox : public ClickableControl {
public:
    ZAF_DECLARE_TYPE

public:
    ComboBox();
    ~ComboBox();

    Frame TextInset() const {
        return text_inset_;
    }

    void SetTextInset(const Frame& inset);

    /**
     Get the color picker of drop down button.
     */
    ColorPicker GetDropDownButtonColorPicker() const;

    /**
     Set the color picker of drop down button.
     */
    void SetDropDownButtonColorPicker(const ColorPicker& color_picker);

    /**
     Get the current color of drop down button.
     */
    Color GetDropDownButtonColor() const {
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
    std::size_t GetMinVisibleItemCount() const;

    /**
     Set the minimum visible item count in drop down list.
     */
    void SetMinVisibleItemCount(std::size_t count);

    /**
     Get the maximum visible item count in drop down list.

     This property affects the maximum height of drop down list. The maximum height
     is equal to the total height of items in specified count.

     The default count is the maximum value of std::size_t.
     */
    std::size_t GetMaxVisibleItemCount() const;

    /**
     Set the maximum visible item count in drop down list.
     */
    void SetMaxVisibleItemCount(std::size_t count);

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
    Observable<ComboBoxSelectionChangeInfo> SelectionChangeEvent();

    /**
     Get the drop down list box used in combo box.
     */
    const std::shared_ptr<ComboBoxDropDownListBox>& GetDropDownListBox() const {
        return drop_down_list_box_;
    }

    /**
     Set a new drop down list box to combo box.

     Setting a nullptr would reset the drop down list box to a default one.
     */
    void SetDropDownListBox(const std::shared_ptr<ComboBoxDropDownListBox>& list_box);

    /**
     Get the edit text box used in combo box.
     */
    const std::shared_ptr<ComboBoxEditTextBox>& GetEditTextBox() const {
        return edit_text_box_;
    }

    /**
     Set a new edit text box to combo box.

     Setting a nullptr would reset the edit text box to a default one.
     */
    void SetEditTextBox(const std::shared_ptr<ComboBoxEditTextBox>& text_box);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    zaf::Rect GetTextRect() override;

    void OnClick() override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnFocusGained(const FocusGainedInfo& event_info) override;

    void OnTextChanged() override;

    /**
     This method is called when the selection is changed.

     Derived classes must call the same method of base class.
     */
    virtual void SelectionChange() { }

    /**
     This method is called when the drop down list box is changed.

     @param previous_drop_down_list_box
         The previous drop down list box, may be nullptr.

     Derived classes must call the same method of base class.
     */
    virtual void DropDownListBoxChange(const std::shared_ptr<ComboBoxDropDownListBox>& previous_drop_down_list_box) { }

    /**
     This method is called when the edit text box is changed.

     @param previous_edit_text_box
         The previous edit text box, may be nullptr.

     Derived classes must call the same method of base class.
     */
    virtual void EditTextBoxChange(const std::shared_ptr<TextBox>& previous_edit_text_box) { }

private:
    enum class DropDownListBoxAction {
        CloseDropDownWindow,
        ChangeText,
        Nothing,
    };

    enum class EditTextBoxAction {
        ChangeText,
        Nothing,
    };

    enum class TextChangeSource {
        None,
        DropDownListBox,
        EditTextBox,
    };

    template<typename ValueType>
    class GuardedValue {
    public:
        class Guard {
        public:
            Guard(ValueType& value) : 
                value_(&value), 
                default_value_(value) {
            
            }

            Guard(Guard&& rhs) :
                value_(rhs.value_),
                default_value_(rhs.default_value_) {
                rhs.value_ = nullptr;
            }

            ~Guard() {
                if (value_ != nullptr) {
                    *value_ = default_value_;
                }
            }

            Guard(const Guard&) = delete;
            Guard& operator=(const Guard&) = delete;

        private:
            ValueType* value_;
            ValueType default_value_;
        };

    public:
        GuardedValue(ValueType default_value) :
            value_(default_value) {

        }

        ValueType Get() const {
            return value_;
        }

        Guard Set(ValueType value) {
            Guard guard(value_);
            value_ = value;
            return guard;
        }

    private:
        ValueType value_;
    };

private:
    void InitializeDropDownListBox();
    void UninitializeDropDownListBox();

    void InitializeEditTextBox();
    void UninitializeEditTextBox();

    void PopupDropDownWindow();
    float CalculateDropDownListHeight(std::size_t visible_item_count);
    void DropDownWindowDestroy();

    void DropDownListBoxMouseMove(const Point& position);
    bool SelectNextDropDownListItem(bool reverse);

    void DropDownListBoxSelectionChange();
    void EditTextBoxTextChange();
    void ConfirmSelection(bool discard_drop_down_list_selection);

    void ChangeSelectionText(const std::wstring& text, TextChangeSource source);
    void NotifySelectionChange();

private:
    std::shared_ptr<ComboBoxEditTextBox> edit_text_box_;
    Subscription edit_text_box_subscription_;
    Frame text_inset_;

    std::shared_ptr<internal::ComboBoxDropDownWindow> drop_down_window_;
    std::shared_ptr<ComboBoxDropDownListBox> drop_down_list_box_;
    Subscription drop_down_list_box_subscription_;

    GuardedValue<DropDownListBoxAction> drop_down_list_box_action_;
    GuardedValue<EditTextBoxAction> edit_text_box_action_;
    GuardedValue<TextChangeSource> text_change_source_;

    std::optional<std::size_t> recovered_selected_index_;
    bool need_recover_selected_index_{};
};


class ComboBoxSelectionChangeInfo {
public:
    ComboBoxSelectionChangeInfo(const std::shared_ptr<ComboBox>& combo_box) : 
        combo_box_(combo_box) {

    }

    const std::shared_ptr<ComboBox>& ComboBox() const {
        return combo_box_;
    }

private:
    std::shared_ptr<zaf::ComboBox> combo_box_;
};


/**
 Represents a drop down list box control used in combo box.
 */
class ComboBoxDropDownListBox : public ListBox {
public:
    ZAF_DECLARE_TYPE

protected:
    void Initialize() override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;

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
class ComboBoxEditTextBox : public TextBox {
public:
    ZAF_DECLARE_TYPE

public:
    void OnKeyDown(const KeyDownInfo& event_info) override;
};

}