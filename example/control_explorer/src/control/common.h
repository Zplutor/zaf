#pragma once

#include <memory>
#include <zaf/control/control.h>

static const float SingleLineOperatePanelHeight = 20;

std::shared_ptr<zaf::Control> CreateOperateContainerPanel(std::size_t lines);