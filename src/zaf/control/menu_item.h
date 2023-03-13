#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

class MenuItem : public TextualControl {
public:

protected:
    void Initialize() override;

private:
    void AdjustAppearence();
};

}