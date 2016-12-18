#pragma once

#include <zaf/control/control.h>

void CreateControlOperatePanels(
    const std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels);