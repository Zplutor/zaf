#pragma once

#include <zaf/control/combo_box.h>
#include "manager/textual_control_explore_manager.h"

class ComboBoxExploreManager : public TextualControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};