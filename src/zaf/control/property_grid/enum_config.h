#pragma once

#include <zaf/control/property_grid/enum_editor.h>
#include <zaf/control/property_grid/type_config.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class EnumConfig : public TypeConfig {
public:
    std::shared_ptr<ValueEditor> CreateValueEditor() override {
        return Create<EnumEditor>();
    }
};

}