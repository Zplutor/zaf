#pragma once

#include <zaf/control/scrollable_control.h>

class ControlOperatePanel : public zaf::Control {
public:
    void Initialize() override;

    void SetOperatePanels(const std::vector<std::shared_ptr<zaf::Control>>& panels);

private:
    std::shared_ptr<zaf::ScrollableControl> scrollable_control_;
};