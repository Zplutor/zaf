#pragma once

#include <zaf/control/check_box.h>
#include "manager/textual_control_explore_manager.h"

class CheckBoxExploreManager : public TextualControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};