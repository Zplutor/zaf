#pragma once

#include <zaf/control/control.h>

namespace zaf {

class SpinButton;
class SpinEdit;

class SpinBox : public zaf::Control {
public:
    ZAF_DECLARE_TYPE;

protected:
    void Initialize() override;

protected:
    void Layout(const zaf::Rect& previous_rect) override;

private:
    std::shared_ptr<SpinEdit> edit_;
    std::shared_ptr<SpinButton> up_button_;
    std::shared_ptr<SpinButton> down_button_;
};

}