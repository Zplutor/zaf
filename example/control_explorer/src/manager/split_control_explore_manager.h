#pragma once

#include <zaf/control/split_control.h>
#include "manager/control_explore_manager.h"

class SplitControlExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::SplitControl> GetSplitControl() {
        return std::dynamic_pointer_cast<zaf::SplitControl>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateSplitBarThicknessPropertyItem();
    std::shared_ptr<PropertyItem> CreateIsHorizontalSplitPropertyItem();
    std::shared_ptr<PropertyItem> CreateSplitBarDistancePropertyItem();
};