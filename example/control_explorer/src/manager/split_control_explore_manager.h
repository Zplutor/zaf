#pragma once

#include <zaf/control/split_control.h>
#include "manager/control_explore_manager.h"

class SplitControlExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};