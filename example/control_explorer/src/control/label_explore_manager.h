#pragma once

#include "control_explore_manager.h"

class LabelExploreManager : public ControlExploreManager {
public:
    void Initialize(
        std::shared_ptr<zaf::Control>& control,
        std::vector<std::shared_ptr<zaf::Control>>& operate_items) override;
};