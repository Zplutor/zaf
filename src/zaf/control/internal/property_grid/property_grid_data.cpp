#include <zaf/control/internal/property_grid/property_grid_data.h>
#include <zaf/base/container/utility/append.h>
#include <zaf/object/object_type.h>

namespace zaf::internal {

PropertyGridData::PropertyGridData(
    zaf::ObjectProperty* property,
    const std::shared_ptr<zaf::Object>& value)
    :
    property_(property),
    value_(value) {

    InspectValueProperties();
}


void PropertyGridData::InspectValueProperties() {

    if (!value_) {
        return;
    }

    std::vector<zaf::ObjectType*> value_type_chain;

    auto value_type = value_->GetType();
    value_type_chain.push_back(value_type);

    while (value_type = value_type->GetBase()) {
        value_type_chain.push_back(value_type);
    }

    std::reverse(value_type_chain.begin(), value_type_chain.end());

    for (auto each_type : value_type_chain) {
        zaf::Append(value_properties_, each_type->GetProperties());
    }
}

}