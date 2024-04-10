#pragma once

#include "manager/textual_control_explore_manager.h"

class TextBoxExploreManager : public TextualControlExploreManager {
public:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};