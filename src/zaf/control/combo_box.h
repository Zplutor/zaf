#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/list_box.h>

namespace zaf {

namespace internal {
class ComboBoxDropDownWindow;
}

class TextBox;

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

    const std::shared_ptr<TextBox>& GetEditTextBox() const {
        return edit_text_box_;
    }

    void SetEditTextBox(const std::shared_ptr<TextBox>& text_box);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    const Rect GetTextRect() const override;

    void MouseClick() override;
    bool KeyDown(const KeyMessage& message) override;

    virtual void DropDownListBoxChange(const std::shared_ptr<ListControl>& previous_drop_down_list_control) { }

private:
    enum class SelectionChangeAction {
        CloseDropDownWindow,
        ChangeText,
        Nothing,
    };

    class SelectionChangeActionGuard {
    public:
        SelectionChangeActionGuard(SelectionChangeAction* action) : action_(action) {
        
        }

        SelectionChangeActionGuard(SelectionChangeActionGuard&& rhs) : action_(rhs.action_) {
            rhs.action_ = nullptr;
        }

        ~SelectionChangeActionGuard() { 
            if (action_ != nullptr) {
                *action_ = SelectionChangeAction::CloseDropDownWindow;
            }
        }

        SelectionChangeActionGuard(const SelectionChangeActionGuard&) = delete;
        SelectionChangeActionGuard& operator=(const SelectionChangeActionGuard&) = delete;

    private:
        SelectionChangeAction* action_;
    };

private:
    void InitializeDropDownListBox();
    void UninitializeDropDownListBox();
    void PopupDropDownWindow();
    float CalculateDropDownListHeight(std::size_t visible_item_count);

    void DropDownListBoxMouseMove(const Point& position);
    bool SelectNextDropDownListItem(bool reverse);

    void DropDownListBoxSelectionChange();
    void NotifySelectionChange();

    SelectionChangeActionGuard SetSelectionChangeAction(SelectionChangeAction action);

private:
    std::shared_ptr<TextBox> edit_text_box_;
    std::shared_ptr<internal::ComboBoxDropDownWindow> drop_down_window_;
    std::shared_ptr<DropDownListBox> drop_down_list_box_;
    SelectionChangeAction selection_change_action_;
};

}