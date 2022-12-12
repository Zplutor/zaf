#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

class ListItem : public TextualControl {
protected:
    void Initialize() override;

private:
    bool IsWithinFocusedControl() const;
};

}