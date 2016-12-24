#include "control/common.h"
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>

std::shared_ptr<zaf::Control> CreateOperateContainerPanel(std::size_t lines) {

    auto panel = zaf::Create<zaf::Control>();
    panel->SetSize(zaf::Size(0, SingleLineOperatePanelHeight * lines));
    panel->SetLayouter(zaf::GetHorizontalArrayLayouter());
    return panel;
}