#pragma once

#include <zaf/control/property_grid/type_config.h>

namespace zaf::property_grid {

class SizeConfig : public TypeConfig {
public:
    void FilterProperties(std::vector<ObjectProperty*>& properties) override;
};

}