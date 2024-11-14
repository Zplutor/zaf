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

private:
    friend class ListBox;

    void SetItemHeight(float height) {
        item_height_ = height;
    }

private:
    float item_height_{};
};

}