#include "control_property_panel.h"
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>

void ControlPropertyPanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    refresh_button_ = zaf::Create<zaf::Button>();
    refresh_button_->SetText(L"Refresh");
    refresh_button_->SetFixedHeight(30);
    refresh_button_->SetIsVisible(false);
    Disposables() += refresh_button_->ClickEvent().Subscribe(std::bind([this]() {
        property_grid_->RefreshValues();
    }));
    AddChild(refresh_button_);

    property_grid_ = zaf::Create<zaf::PropertyGrid>();
    property_grid_->SetBorder(zaf::Frame{});
    property_grid_->SetAutoHideScrollBars(true);
    property_grid_->SetIsVisible(false);
    AddChild(property_grid_);
}


void ControlPropertyPanel::SetExploredControl(const std::shared_ptr<zaf::Control>& control) {

    refresh_button_->SetIsVisible(true);
    property_grid_->SetIsVisible(true);
    property_grid_->SetTargetObject(control);
}
