#pragma once

#include <zaf/control/control.h>

class ControlViewPanel : public zaf::Control {
public:
    void SetExploredControl(const std::shared_ptr<zaf::Control>& control);

protected:
    void Layout(const zaf::Rect&) override;

private:
    void ExploredControlRectChanged(const zaf::ControlRectChangeInfo& event_info);

private:
    std::shared_ptr<zaf::Control> explored_control_;
    zaf::Subscription explored_control_subscription_;
};