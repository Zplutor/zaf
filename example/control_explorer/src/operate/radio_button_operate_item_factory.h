#pragma once

#include <zaf/control/radio_button.h>
#include "operate/textual_control_operate_item_factory.h"

class RadioButtonOperateItemFactory : public TextualControlOperateItemFactory {
public:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    std::shared_ptr<zaf::RadioButton> GetRadioButton() const {
        return std::dynamic_pointer_cast<zaf::RadioButton>(GetTargetControl());
    }

    std::shared_ptr<zaf::Control> CreateCanAutoSelectOperateItem();
    std::shared_ptr<zaf::Control> CreateIsSelectedOperateItem();
};