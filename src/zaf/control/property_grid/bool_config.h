#pragma once

#include <zaf/control/property_grid/bool_value_view.h>
#include <zaf/control/property_grid/type_config.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class BoolConfig : public TypeConfig {
public:
    std::shared_ptr<ValueView> CreateValueView() override {
        return Create<BoolValueView>();
    }
};

}
