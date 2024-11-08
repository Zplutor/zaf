#pragma once

#include <zaf/control/property_grid/bool_editor.h>
#include <zaf/control/property_grid/type_config.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class BoolConfig : public TypeConfig {
public:
    std::shared_ptr<ValueEditor> CreateValueView() override {
        return Create<BoolEditor>();
    }
};

}
