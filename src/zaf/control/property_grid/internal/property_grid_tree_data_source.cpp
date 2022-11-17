#include <zaf/control/property_grid/internal/property_grid_tree_data_source.h>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/append.h>
#include <zaf/creation.h>

namespace zaf::internal {

PropertyGridTreeDataSource::PropertyGridTreeDataSource(
    const std::shared_ptr<Object>& target_object,
    const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory)
    :
    type_config_factory_(type_config_factory) {

    ZAF_EXPECT(target_object);
    ZAF_EXPECT(type_config_factory_);
    root_data_ = CreateData(nullptr, target_object);
}


std::shared_ptr<PropertyGridData> PropertyGridTreeDataSource::CreateData(
    ObjectProperty* property,
    const std::shared_ptr<Object>& value) const {

    return std::make_shared<PropertyGridData>(property, value, InspectProperties(value));
}


std::vector<ObjectProperty*> PropertyGridTreeDataSource::InspectProperties(
    const std::shared_ptr<Object>& value) const {

    if (!value) {
        return {};
    }

    std::vector<ObjectType*> value_type_chain;

    auto value_type = value->GetType();
    value_type_chain.push_back(value_type);

    while (value_type = value_type->GetBase()) {
        value_type_chain.push_back(value_type);
    }

    std::reverse(value_type_chain.begin(), value_type_chain.end());

    std::vector<ObjectProperty*> result;
    for (auto each_type : value_type_chain) {
        zaf::Append(result, each_type->GetProperties());
    }

    auto type_config = type_config_factory_->GetConfig(value->GetType());
    type_config->FilterProperties(result);
    return result;
}


bool PropertyGridTreeDataSource::DoesDataHasChildren(const std::shared_ptr<Object>& data) {

    auto target_data = data ? As<PropertyGridData>(data) : root_data_;
    return !target_data->ValueProperties().empty();
}


std::size_t PropertyGridTreeDataSource::GetChildDataCount(
    const std::shared_ptr<Object>& parent_data) {

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

    return CreateData(property, value);
}


}