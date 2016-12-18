#pragma once

#include <zaf/control/control.h>

class ControlViewPanel : public zaf::Control {
protected:
    void Layout(const zaf::Rect&) override;
};