#pragma once

#include <zaf/control/combo_box.h>
#include "operate/textual_control_operate_item_factory.h"

class ComboBoxOperateItemFactory : public TextualControlOperateItemFactory {
public:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    std::shared_ptr<zaf::ComboBox> GetComboBox() const {
        return std::dynamic_pointer_cast<zaf::ComboBox>(GetTargetControl());
    }

    std::shared_ptr<zaf::Control> CreateIsEditableOperateItem();
    std::shared_ptr<zaf::Control> CreateDropDownButtonWidthOperateItem();
    std::shared_ptr<zaf::Control> CreateMinimumVisibleItemCountOperateItem();
    std::shared_ptr<zaf::Control> CreateMaximumVisibleItemCountOperateItem();
};