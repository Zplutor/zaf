#pragma once

#include <memory>
#include <zaf/control/button.h>
#include <zaf/control/control.h>

const float SingleLineOperatePanelHeight = 20;
const float OperatePanelLabelMaximumWidth = 100;

std::shared_ptr<zaf::Control> CreateOperateContainerPanel(std::size_t lines);
std::shared_ptr<zaf::Button> CreateSetButton();