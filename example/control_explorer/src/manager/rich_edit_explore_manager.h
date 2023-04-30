#pragma once

#include <zaf/control/rich_edit.h>
#include "manager/textual_control_explore_manager.h"

class RichEditExploreManager : public TextualControlExploreManager {
public:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};