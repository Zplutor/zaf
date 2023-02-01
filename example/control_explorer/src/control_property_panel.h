#pragma once

#include <zaf/control/button.h>
#include <zaf/control/property_grid.h>

class ControlPropertyPanel : public zaf::Control {
public:
    void Initialize() override;

    void SetExploredControl(const std::shared_ptr<zaf::Control>& control);

private:
    std::shared_ptr<zaf::Button> refresh_button_;
    std::shared_ptr<zaf::PropertyGrid> property_grid_;
};