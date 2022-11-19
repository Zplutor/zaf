#pragma once

#include <zaf/control/control.h>

namespace zaf::property_grid {

enum class AccessMethod {
    ReadWrite,
    ReadOnly,
};

class ValueView : public Control {
public:
    virtual void SetAccessMethod(AccessMethod) = 0;
    virtual void SetValue(const std::shared_ptr<Object>& object) = 0;

protected:
    void Initialize() override;
};

}