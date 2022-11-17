#pragma once

#include <zaf/control/control.h>

namespace zaf::property_grid {

class ValueView : public Control {
public:
    virtual void SetValue(const std::shared_ptr<Object>& object) = 0;

protected:
    void Initialize() override;
};

}