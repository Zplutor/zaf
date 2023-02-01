#pragma once

#include <zaf/control/scroll_bar.h>
#include "manager/control_explore_manager.h"

class ScrollBarExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};