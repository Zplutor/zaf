#pragma once

#include <zaf/control/combo_box.h>
#include "manager/textual_control_explore_manager.h"

class ComboBoxExploreManager : public TextualControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::ComboBox> GetComboBox() {
        return std::dynamic_pointer_cast<zaf::ComboBox>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateIsEditablePropertyItem();
    std::shared_ptr<PropertyItem> CreateDropDownButtonWidthPropertyItem();
    std::shared_ptr<PropertyItem> CreateMinimumVisibleItemCountPropertyItem();
    std::shared_ptr<PropertyItem> CreateMaximumVisibleItemCountPropertyItem();
};