#pragma once

#include <zaf/control/control.h>

class ControlViewPanel : public zaf::Control {
public:
    ~ControlViewPanel();
    void SetExploredControl(const std::shared_ptr<zaf::Control>& control);

protected:
    void Layout(const zaf::Rect&) override;

private:
    void ExploredControlRectChanged(
        const std::shared_ptr<zaf::Control>& control,
        const zaf::Rect& previous_rect);

private:
    std::shared_ptr<zaf::Control> explored_control_;
};