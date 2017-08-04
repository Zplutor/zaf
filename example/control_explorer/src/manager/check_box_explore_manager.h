#pragma once

#include <zaf/control/check_box.h>
#include "manager/textual_control_explore_manager.h"

class CheckBoxExploreManager : public TextualControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::CheckBox> GetCheckBox() {
        return std::dynamic_pointer_cast<zaf::CheckBox>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateCanAutoChangeCheckStatePropertyItem();
    std::shared_ptr<PropertyItem> CreateCanBeIndeterminatePropertyItem();
    std::shared_ptr<PropertyItem> CreateIsCheckedPropertyItem();
    std::shared_ptr<PropertyItem> CreateCheckStatePropertyItem();
};