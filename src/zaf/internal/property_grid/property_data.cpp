#include <zaf/internal/property_grid/property_data.h>
#include <zaf/base/auto_reset.h>

namespace zaf::internal {

PropertyData::PropertyData(
    std::shared_ptr<PropertyData> parent,
    zaf::ObjectProperty* property,
    const std::shared_ptr<zaf::Object>& value,
    const std::shared_ptr<PropertyGridDelegate>& delegate)
    :
    parent_(std::move(parent)),
    property_(property),
    value_(value),
    delegate_(delegate) {

}


bool PropertyData::IsReadOnly() const {

    if (property_ && !property_->CanSet()) {
        return true;
    }

    auto parent = parent_.lock();
    if (!parent) {
        return false;
    }

    return parent->IsReadOnly();
}


const std::vector<std::shared_ptr<PropertyData>>& PropertyData::Children() {

    if (!children_) {
        children_ = LoadChildren();
    }

    return *children_;
}


std::vector<std::shared_ptr<PropertyData>> PropertyData::LoadChildren() {

    if (!value_) {
        return {};
    }

    std::vector<ObjectType*> value_type_chain = GetObjectTypeChain(*value_);
    property_grid::PropertyTable property_table = CreatePropertyTable(value_type_chain);

    auto type_config = delegate_->GetTypeConfig(value_->DynamicType());
    type_config->FilterProperties(property_table);

    std::vector<std::shared_ptr<PropertyData>> result;
    for (auto& each_pair : property_table.Inner()) {
        for (auto each_property : each_pair.second.Inner()) {

            auto child_data = std::make_shared<PropertyData>(
                shared_from_this(),
                each_property,
                each_property->GetValue(*value_), 
                delegate_);

            result.push_back(child_data);
        }
    }
    return result;
}


std::vector<ObjectType*> PropertyData::GetObjectTypeChain(const Object& object) {

    std::vector<ObjectType*> type_chain;

    auto type = object.DynamicType();
    type_chain.push_back(type);

    while (type = type->BaseType()) {
        type_chain.push_back(type);
    }

    std::reverse(type_chain.begin(), type_chain.end());
    return type_chain;
}


property_grid::PropertyTable PropertyData::CreatePropertyTable(
    const std::vector<ObjectType*>& types) {

    std::vector<std::pair<ObjectType*, property_grid::PropertyList>> property_table_inner;
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

    return property_grid::PropertyTable(std::move(property_table_inner));
}


void PropertyData::ChangeValue(std::shared_ptr<Object> new_value) {

    auto auto_reset = MakeAutoReset(is_changing_value_, true);

    SetValue(std::move(new_value));
    PropagateValueChangedToChildren();
    PropagateValueChangedToParent();
}


void PropertyData::SetValue(std::shared_ptr<Object> new_value) {
    value_ = std::move(new_value);
    value_changed_event_.AsObserver().OnNext(shared_from_this());
}


void PropertyData::PropagateValueChangedToParent() {

    auto parent = parent_.lock();
    if (parent) {
        parent->ChangeValueFromChild(*this);
    }
    else {
        PropagateValueChangedToChildren();
    }
}


void PropertyData::ChangeValueFromChild(const PropertyData& child) {

    child.Property()->SetValue(*value_, child.Value());
    value_changed_event_.AsObserver().OnNext(shared_from_this());

    PropagateValueChangedToParent();
}


void PropertyData::PropagateValueChangedToChildren() {

    if (!children_) {
        return;
    }

    for (const auto& each_child : *children_) {

        std::shared_ptr<Object> new_child_value;
        if (value_) {
            new_child_value = each_child->Property()->GetValue(*value_);
        }

        each_child->ChangeValueFromParent(std::move(new_child_value));
    }
}


void PropertyData::ChangeValueFromParent(std::shared_ptr<Object> new_value) {

    if (is_changing_value_) {
        return;
    }

    SetValue(std::move(new_value));
    PropagateValueChangedToChildren();
}

}