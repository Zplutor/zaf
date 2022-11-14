#include <zaf/control/internal/property_grid/property_grid_tree_data_source.h>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/append.h>
#include <zaf/creation.h>

namespace zaf::internal {

PropertyGridTreeDataSource::PropertyGridTreeDataSource(const std::shared_ptr<Object>& target_object) :
    root_data_(Create<PropertyGridData>(nullptr, target_object)) {

}


bool PropertyGridTreeDataSource::DoesDataHasChildren(const std::shared_ptr<Object>& data) {

    auto target_data = data ? As<PropertyGridData>(data) : root_data_;
    return !target_data->ValueProperties().empty();
}


std::size_t PropertyGridTreeDataSource::GetChildDataCount(const std::shared_ptr<Object>& parent_data) {

    auto target_data = parent_data ? As<PropertyGridData>(parent_data) : root_data_;
    return target_data->ValueProperties().size();
}


std::shared_ptr<Object> PropertyGridTreeDataSource::GetChildDataAtIndex(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index) {

    auto target_data = parent_data ? As<PropertyGridData>(parent_data) : root_data_;

    const auto& value_properties = target_data->ValueProperties();
    ZAF_EXPECT(index < value_properties.size());

    auto property = value_properties[index];

    std::shared_ptr<Object> value;
    if (property->CanGet()) {
        value = property->GetValue(*target_data->Value());
    }

    return Create<PropertyGridData>(property, value);
}

}