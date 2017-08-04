#pragma once

#include "manager/textual_control_explore_manager.h"

class ButtonExploreManager : public TextualControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};