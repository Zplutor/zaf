#pragma once

#include <zaf/control/textual_control.h>
#include "manager/control_explore_manager.h"

class TextualControlExploreManager : public ControlExploreManager {
protected:
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::TextualControl> GetTextualControl() {
        return std::dynamic_pointer_cast<zaf::TextualControl>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateTextAlignmentPropertyItem();
    std::shared_ptr<PropertyItem> CreateParagraphAlignmentPropertyItem();
    std::shared_ptr<PropertyItem> CreateWordWrappingPropertyItem();
    std::shared_ptr<PropertyItem> CreateTextPropertyItem();
};