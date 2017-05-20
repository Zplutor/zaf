#pragma once

#include <zaf/control/check_box.h>
#include "operate/textual_control_operate_item_factory.h"

class CheckBoxOperateItemFactory : public TextualControlOperateItemFactory {
public:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    std::shared_ptr<zaf::CheckBox> GetCheckBox() const {
        return std::dynamic_pointer_cast<zaf::CheckBox>(GetTargetControl());
    }

    std::shared_ptr<zaf::Control> CreateCanAutoChangeCheckStateOperateItem();
    std::shared_ptr<zaf::Control> CreateCanBeIndeterminateOperateItem();
    std::shared_ptr<zaf::Control> CreateIsCheckedOperateItem();
    std::shared_ptr<zaf::Control> CreateCheckStateOperateItem();
};