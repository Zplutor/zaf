#include <zaf/internal/property_grid/property_grid_data.h>

namespace zaf::property_grid::internal {

PropertyGridData::PropertyGridData(
    zaf::ObjectProperty* property,
    const std::shared_ptr<zaf::Object>& value,
    bool is_parent_read_only,
    const std::shared_ptr<TypeConfigFactory>& type_config_factory,
    const std::weak_ptr<PropertyGridDataObserver>& observer)
    :
    property_(property),
    value_(value),
    type_config_factory_(type_config_factory),
    observer_(observer) {

    bool is_self_read_only = property ? !property->CanSet() : false;
    is_read_only_ = is_parent_read_only || is_self_read_only;
}


const std::vector<std::shared_ptr<PropertyGridData>>& PropertyGridData::Children() {

    if (!children_) {
        children_ = LoadChildren();
    }

    return *children_;
}


std::vector<std::shared_ptr<PropertyGridData>> PropertyGridData::LoadChildren() {

    if (!value_) {
        return {};
    }

    std::vector<ObjectType*> value_type_chain = GetObjectTypeChain(*value_);
    property_grid::PropertyTable property_table = CreatePropertyTable(value_type_chain);

    auto type_config = type_config_factory_->GetConfig(value_->DynamicType());
    type_config->FilterProperties(property_table);

    std::vector<std::shared_ptr<PropertyGridData>> result;
    for (auto& each_pair : property_table.Inner()) {
        for (auto each_property : each_pair.second.Inner()) {

            auto child_data = std::make_shared<PropertyGridData>(
                each_property,
                each_property->GetValue(*value_), 
                is_read_only_,
                type_config_factory_,
                observer_);

            Subscriptions() += child_data->ValueChangedEvent().Subscribe(
                std::bind(&PropertyGridData::OnChildValueChanged, this, std::placeholders::_1));

            result.push_back(child_data);
        }
    }
    return result;
}


std::vector<ObjectType*> PropertyGridData::GetObjectTypeChain(const Object& object) {

    std::vector<ObjectType*> type_chain;

    auto type = object.DynamicType();
    type_chain.push_back(type);

    while (type = type->BaseType()) {
        type_chain.push_back(type);
    }

    std::reverse(type_chain.begin(), type_chain.end());
    return type_chain;
}


PropertyTable PropertyGridData::CreatePropertyTable(
    const std::vector<ObjectType*>& types) {

    std::vector<std::pair<ObjectType*, PropertyList>> property_table_inner;
    for (auto each_type : types) {

        std::vector<ObjectProperty*> property_list_inner;
        for (auto each_property : each_type->NonInheritedProperties()) {
            //Write only properties are not supported.
            if (each_property->CanGet()) {
                property_list_inner.push_back(each_property);
            }
        }

        property_table_inner.emplace_back(each_type, std::move(property_list_inner));
    }

    return PropertyTable(std::move(property_table_inner));
}


void PropertyGridData::ChangeValueFromUpToDown(const std::shared_ptr<Object>& value) {

    value_ = value;

    if (!value_ || !children_) {
        return;
    }

    for (const auto& each_child : *children_) {

        auto new_value = each_child->Property()->GetValue(*value_);
        each_child->ChangeValueFromUpToDown(new_value);
    }

    if (!children_->empty()) {
        if (auto observer = observer_.lock()) {
            observer->OnDataChildrenUpdate(shared_from_this(), children_->size());
        }
    }
}


void PropertyGridData::ChangeValueFromDownToUp(const std::shared_ptr<Object>& value) {

    value_ = value;

    value_changed_subject_.AsObserver().OnNext(shared_from_this());
}


void PropertyGridData::OnChildValueChanged(const std::shared_ptr<PropertyGridData>& child) {

    child->Property()->SetValue(*value_, child->Value());

    if (property_) {
        value_changed_subject_.AsObserver().OnNext(shared_from_this());
    }
    else {
        ChangeValueFromUpToDown(value_);
    }
}

}