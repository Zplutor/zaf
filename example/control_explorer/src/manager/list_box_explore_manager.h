#pragma once

#include <zaf/control/list_box.h>
#include "manager/control_explore_manager.h"

class ListBoxExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::ListBox> GetListBox() {
        return std::dynamic_pointer_cast<zaf::ListBox>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateDefaultItemHeightPropertyItem();
};