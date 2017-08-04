#pragma once

#include <zaf/control/scrollable_control.h>

class PropertyItem;

class ControlPropertyPanel : public zaf::Control {
public:
    void Initialize() override;

    void SetPropertyItems(const std::vector<std::shared_ptr<PropertyItem>>& items);

private:
    std::shared_ptr<zaf::ScrollableControl> scrollable_control_;
};