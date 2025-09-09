#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/property_grid/type_config.h>
#include <zaf/dynamic/object_type.h>

namespace zaf {

class PropertyGridDelegate : NonCopyableNonMovable {
public:
    virtual ~PropertyGridDelegate() = default;

    virtual std::shared_ptr<property_grid::TypeConfig> GetTypeConfig(dynamic::ObjectType* type);
};

}
