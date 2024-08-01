#pragma once

#include <optional>
#include <zaf/control/clickable_control.h>
#include <zaf/control/event/combo_box_selection_changed_info.h>
#include <zaf/control/list_box.h>
#include <zaf/control/list_box_delegate.h>
#include <zaf/control/rich_edit.h>
#include <zaf/rx/observable.h>

namespace zaf {
namespace internal {
class ComboBoxDropDownWindow;
}

class ComboBoxDropDownListBox;
class ComboBoxEditBox;

/**
 Represents a combo box control.
 */
class ComboBox : public ClickableControl {
public:
    ZAF_OBJECT;

public:
    ComboBox();
    ~ComboBox();

    Frame TextInset() const {
        return text_inset_;
    }

    void SetTextInset(const Frame& inset);

    /**
     Get the current color of drop down button.
     */
    Color DropDownButtonColor() const;

    /**
     Set the color of drop down button.
     */
    void SetDropDownButtonColor(const Color& color);

    /**
     Get the width of drop down button.

     The default width is 12.
     */
    float DropDownButtonWidth() const;

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
    std::size_t MinVisibleItemCount() const;

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
    std::size_t MaxVisibleItemCount() const;

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
    Observable<ComboBoxSelectionChangedInfo> SelectionChangedEvent() const;

    /**
     Get the drop down list box used in combo box.
     */
    const std::shared_ptr<ComboBoxDropDownListBox>& DropDownListBox() const {
        return drop_down_list_box_;
    }

    /**
     Set a new drop down list box to combo box.

     Setting a nullptr would reset the drop down list box to a default one.
     */
    void SetDropDownListBox(const std::shared_ptr<ComboBoxDropDownListBox>& list_box);

    /**
     Get the edit box used in combo box.
     */
    const std::shared_ptr<ComboBoxEditBox>& EditBox() const {
        return edit_box_;
    }

    /**
     Set a new edit box to combo box.

     Setting a nullptr would reset the edit box to a default one.
     */
    void SetEditBox(const std::shared_ptr<ComboBoxEditBox>& edit_box);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    zaf::Rect DetermineTextRect() override;

    void OnClick(const ClickInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnFocusGained(const FocusGainedInfo& event_info) override;

    void OnTextChanged(const TextChangedInfo& event_info) override;

    /**
    Handles selection changed notification. This method would be called after the selection of 
    combo box is changed.

    The default implementation raises selection changed event. Derived classes should call the same
    method of base class to raise the event if they override it.
    */
    virtual void OnSelectionChanged(const ComboBoxSelectionChangedInfo& event_info);

    /**
     This method is called when the drop down list box is changed.

     @param previous_drop_down_list_box
         The previous drop down list box, may be nullptr.

     Derived classes must call the same method of base class.
     */
    virtual void OnDropDownListBoxChanged(
        const std::shared_ptr<ComboBoxDropDownListBox>& previous_drop_down_list_box) { }

    /**
     This method is called when the edit box is changed.

     @param previous_edit_box
         The previous edit box, may be nullptr.

     Derived classes must call the same method of base class.
     */
    virtual void OnEditBoxChanged(const std::shared_ptr<RichEdit>& previous_edit_box) { }

private:
    enum class DropDownListBoxAction {
        CloseDropDownWindow,
        ChangeText,
        Nothing,
    };

    enum class EditBoxAction {
        ChangeText,
        Nothing,
    };

    enum class TextChangeSource {
        None,
        DropDownListBox,
        EditBox,
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

    void InitializeEditBox();
    void UninitializeEditBox();

    void PopupDropDownWindow();
    float CalculateDropDownListHeight(std::size_t visible_item_count);
    void DropDownWindowDestroy();

    void DropDownListBoxMouseMove(const Point& position);
    bool SelectNextDropDownListItem(bool reverse);

    void DropDownListBoxSelectionChange();
    void EditBoxTextChange();
    void ConfirmSelection(bool discard_drop_down_list_selection);

    void ChangeSelectionText(const std::wstring& text, TextChangeSource source);
    void NotifySelectionChange();

private:
    bool is_editable_{ false };
    std::shared_ptr<ComboBoxEditBox> edit_box_;
    Subscription edit_box_subscription_;
    Frame text_inset_;

    Color drop_down_button_color_;
    float drop_down_button_width_{ 12 };
    std::size_t min_visible_item_count_{ 1 };
    std::size_t max_visible_item_count_{ (std::numeric_limits<std::size_t>::max)() };
    std::shared_ptr<internal::ComboBoxDropDownWindow> drop_down_window_;
    std::shared_ptr<ComboBoxDropDownListBox> drop_down_list_box_;
    Subscription drop_down_list_box_subscription_;

    GuardedValue<DropDownListBoxAction> drop_down_list_box_action_;
    GuardedValue<EditBoxAction> edit_box_action_;
    GuardedValue<TextChangeSource> text_change_source_;

    std::optional<std::size_t> recovered_selected_index_;
    bool need_recover_selected_index_{};

    Event<ComboBoxSelectionChangedInfo> selection_changed_event_;
};


/**
 Represents a drop down list box control used in combo box.
 */
class ComboBoxDropDownListBox : public ListBox {
public:
    ZAF_OBJECT;

protected:
    void Initialize() override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;

private:
    class DropDownListBoxDelegate : public ListBoxDelegate {
    public:
        std::shared_ptr<ListItem> CreateItem(
            std::size_t item_index,
            const std::shared_ptr<Object>& item_data) override;
    };

private:
    friend class ComboBox;

    void SetMouseMoveCallback(const std::function<void(const Point&)>& callback) {
        mouse_move_callback_ = callback;
    }

private:
    std::function<void(const Point&)> mouse_move_callback_;
};


/**
 Represents an edit box control used in combo box.
 */
class ComboBoxEditBox : public RichEdit {
public:
    ZAF_OBJECT;

public:
    void OnKeyDown(const KeyDownInfo& event_info) override;
};


ZAF_OBJECT_BEGIN(ComboBox);
ZAF_OBJECT_PROPERTY(DropDownButtonColor);
ZAF_OBJECT_PROPERTY(DropDownButtonWidth);
ZAF_OBJECT_PROPERTY(DropDownListBox);
ZAF_OBJECT_PROPERTY(EditBox);
ZAF_OBJECT_PROPERTY(MinVisibleItemCount);
ZAF_OBJECT_PROPERTY(MaxVisibleItemCount);
ZAF_OBJECT_END;

ZAF_OBJECT_BEGIN(ComboBoxDropDownListBox);
ZAF_OBJECT_END;

ZAF_OBJECT_BEGIN(ComboBoxEditBox);
ZAF_OBJECT_END;

}