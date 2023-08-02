#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class SpinBox;

class SpinBoxValueChangedInfo : public EventInfo {
public:
    SpinBoxValueChangedInfo(const std::shared_ptr<SpinBox>& source, int previous_value);

    int PreviousValue() const {
        return previous_value_;
    }

private:
    int previous_value_{};
};

}