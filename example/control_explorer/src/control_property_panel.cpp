#include "control_property_panel.h"
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>

void ControlPropertyPanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    property_grid_ = zaf::Create<zaf::PropertyGrid>();
    property_grid_->SetBorder(zaf::Frame{});
    property_grid_->SetAutoHideScrollBars(true);

    AddChild(property_grid_);
}


void ControlPropertyPanel::SetExploredControl(const std::shared_ptr<zaf::Control>& control) {

    property_grid_->SetTargetObject(control);
}
