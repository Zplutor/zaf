#pragma once

#include <zaf/control/radio_button.h>
#include "manager/textual_control_explore_manager.h"

class RadioButtonExploreManager : public TextualControlExploreManager {
public:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;

    std::shared_ptr<zaf::Control> GetTargetControl() override {
        return target_radio_button_;
    }

private:
    std::shared_ptr<zaf::RadioButton> target_radio_button_;
};