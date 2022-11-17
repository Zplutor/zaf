#pragma once

#include <zaf/control/label.h>
#include <zaf/control/property_grid/value_view.h>

namespace zaf::property_grid {

class TextValueView : public ValueView {
public:
    void SetValue(const std::shared_ptr<Object>& value) override;

protected:
    void Initialize() override;

private:
    std::shared_ptr<Label> label_;
};

}