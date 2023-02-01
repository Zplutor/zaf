#pragma once

#include <zaf/control/list_box.h>
#include "manager/control_explore_manager.h"

class ListBoxExploreManager : public ControlExploreManager {
protected:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};