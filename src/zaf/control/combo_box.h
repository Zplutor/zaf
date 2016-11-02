#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class ListBox;
class TextBox;

class ComboBox : public ClickableControl {
public:
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

    bool GetIsEditable() const;
    void SetIsEditable(bool is_editable);

    const std::shared_ptr<ListBox>& GetDropDownList() const {
        return drop_down_list_;
    }

    void SetDropDownList(const std::shared_ptr<ListBox>& list);

    const std::shared_ptr<TextBox>& GetEditTextBox() const {
        return edit_text_box_;
    }

    void SetEditTextBox(const std::shared_ptr<TextBox>& text_box);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;

    void MouseClick() override;

private:
    void PopupDropDownWindow();

private:
    std::shared_ptr<TextBox> edit_text_box_;
    std::shared_ptr<Window> drop_down_window_;
    std::shared_ptr<ListBox> drop_down_list_;
};

}