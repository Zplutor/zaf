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

    std::vector<ObjectType*> value_type_chain = GetObjectTypeChain(*value);
    property_grid::PropertyTable property_table = CreatePropertyTable(value_type_chain);

    auto type_config = type_config_factory_->GetConfig(value->GetType());
    type_config->FilterProperties(property_table);
        
    std::vector<ObjectProperty*> result;
    for (auto& each_pair : property_table.Inner()) {
        zaf::Append(result, each_pair.second.Inner());
    }
    return result;
}


std::vector<ObjectType*> PropertyGridTreeDataSource::GetObjectTypeChain(const Object& object) {

    std::vector<ObjectType*> type_chain;

    auto type = object.GetType();
    type_chain.push_back(type);

    while (type = type->GetBase()) {
        type_chain.push_back(type);
    }

    std::reverse(type_chain.begin(), type_chain.end());
    return type_chain;
}


property_grid::PropertyTable PropertyGridTreeDataSource::CreatePropertyTable(
    const std::vector<ObjectType*>& types) {

    std::vector<std::pair<ObjectType*, property_grid::PropertyList>> property_table_inner;
    for (auto each_type : types) {

        std::vector<ObjectProperty*> property_list_inner;
        for (auto each_property : each_type->GetProperties()) {
            //Write only properties are not supported.
            if (each_property->CanGet()) {
                property_list_inner.push_back(each_property);
            }
        }

        property_table_inner.emplace_back(each_type, std::move(property_list_inner));
    }

    return property_grid::PropertyTable(std::move(property_table_inner));
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