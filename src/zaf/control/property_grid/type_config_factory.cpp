#include <zaf/control/property_grid/type_config_factory.h>

namespace zaf::property_grid {

std::shared_ptr<TypeConfig> TypeConfigFactory::GetConfig(ObjectType* type) {
    
    static auto default_config = std::make_shared<TypeConfig>();
    return default_config;
}

}