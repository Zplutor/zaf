#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/property_grid/type_config.h>
#include <zaf/object/object_type.h>

namespace zaf::property_grid {

class TypeConfigFactory : NonCopyable {
public:
    TypeConfigFactory() = default;
    virtual ~TypeConfigFactory() = default;

    virtual std::shared_ptr<TypeConfig> GetConfig(ObjectType* type);
};

}