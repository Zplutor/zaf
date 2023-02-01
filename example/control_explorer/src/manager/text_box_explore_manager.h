#pragma once

#include <zaf/control/text_box.h>
#include "manager/textual_control_explore_manager.h"

class TextBoxExploreManager : public TextualControlExploreManager {
public:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
};