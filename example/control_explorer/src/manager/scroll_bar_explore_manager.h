#pragma once

#include <zaf/control/scroll_bar.h>
#include "manager/control_explore_manager.h"

class ScrollBarExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::ScrollBar> GetScrollBar() {
        return std::dynamic_pointer_cast<zaf::ScrollBar>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateIsHorizontalPropertyItem();
    std::shared_ptr<PropertyItem> CreateArrowLengthPropertyItem();
    std::shared_ptr<PropertyItem> CreateMinimumValuePropertyItem();
    std::shared_ptr<PropertyItem> CreateMaximumValuePropertyItem();
    std::shared_ptr<PropertyItem> CreateValuePropertyItem();
    std::shared_ptr<PropertyItem> CreateSmallChangeValuePropertyItem();
    std::shared_ptr<PropertyItem> CreateLargeChangeValuePropertyItem();
};