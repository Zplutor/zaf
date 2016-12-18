#pragma once

#include <zaf/control/textual_control.h>

void CreateTextualControlOperatePanels(
    const std::shared_ptr<zaf::TextualControl>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels);