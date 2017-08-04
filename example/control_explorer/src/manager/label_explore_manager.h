#pragma once

#include "manager/textual_control_explore_manager.h"

class LabelExploreManager : public TextualControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};