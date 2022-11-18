#include <zaf/control/property_grid/size_config.h>

namespace zaf::property_grid {

void SizeConfig::FilterProperties(std::vector<ObjectProperty*>& properties) {

    ArrangeProperties(properties, {
        L"Width",
        L"Height",
    });
}

}