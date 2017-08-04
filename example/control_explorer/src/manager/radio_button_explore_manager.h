#pragma once

#include <zaf/control/radio_button.h>
#include "manager/textual_control_explore_manager.h"

class RadioButtonExploreManager : public TextualControlExploreManager {
public:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

protected:
    std::shared_ptr<zaf::Control> GetTargetControl() override {
        return target_radio_button_;
    }

private:
    std::shared_ptr<zaf::RadioButton> GetRadioButton() {
        return target_radio_button_;
    }

    std::shared_ptr<PropertyItem> CreateCanAutoSelectPropertyItem();
    std::shared_ptr<PropertyItem> CreateIsSelectedPropertyItem();

private:
    std::shared_ptr<zaf::RadioButton> target_radio_button_;
};