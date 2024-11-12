#pragma once

#include "manager/control_explore_manager.h"

class TrackBarExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};