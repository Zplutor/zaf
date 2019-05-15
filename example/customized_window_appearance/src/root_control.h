#pragma once

#include <zaf/control/button.h>
#include <zaf/control/control.h>

class RootControl : public zaf::Control {
public:
    void Initialize() override;
    std::optional<zaf::HitTestResult> HitTest(const zaf::HitTestMessage&) override;

private:
    std::shared_ptr<zaf::Button> close_button_;
};