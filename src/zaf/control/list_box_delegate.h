#pragma once

#include <zaf/control/list_control_delegate.h>

namespace zaf {

class ListBoxDelegate : public ListControlDelegate {
public:
    bool HasVariableItemHeight() override {
        return false;
    }

    float EstimateItemHeight(
        std::size_t item_index, 
        const std::shared_ptr<Object>& item_data) override;

    std::shared_ptr<ListItem> CreateItem(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

private:
    friend class ListBox;

    void SetItemHeight(float height) {
        item_height_ = height;
    }

    void SetTextColorPicker(ColorPicker color_picker) {
        text_color_picker_ = std::move(color_picker);
    }

    void SetBackgroundColorPicker(ColorPicker color_picker) {
        background_color_picker_ = std::move(color_picker);
    }

private:
    float item_height_{};
    ColorPicker text_color_picker_;
    ColorPicker background_color_picker_;
};

}