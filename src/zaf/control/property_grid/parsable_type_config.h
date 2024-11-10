#pragma once

#include <zaf/control/property_grid/type_config.h>
#include <zaf/control/property_grid/parsable_value_editor.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class ParsableTypeConfig : public TypeConfig {
public:
    std::shared_ptr<ValueEditor> CreateValueEditor() override {
        return Create<ParsableValueEditor>();
    }
};

}