#pragma once

#include <zaf/control/property_grid/type_config.h>

namespace zaf::property_grid {

class SizeConfig : public TypeConfig {
public:
    void FilterProperties(PropertyTable& property_table) override;
};

}