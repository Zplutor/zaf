#pragma once

#include <zaf/control/button.h>
#include <zaf/control/control.h>
#include "title_bar_button.h"

class RootControl : public zaf::Control {
public:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    std::optional<zaf::HitTestResult> HitTest(const zaf::HitTestMessage&) override;

private:
    std::shared_ptr<TitleBarButton> close_button_;
    std::shared_ptr<TitleBarButton> minimize_button_;
    std::shared_ptr<TitleBarButton> maximize_button_;

    std::shared_ptr<Control> content_control_;
};