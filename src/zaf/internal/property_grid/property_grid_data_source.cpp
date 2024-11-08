#include <zaf/internal/property_grid/property_grid_data_source.h>
#include <zaf/base/as.h>

namespace zaf::internal {

PropertyGridDataSource::PropertyGridDataSource(const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory) :
    type_config_factory_(type_config_factory) {

    ZAF_EXPECT(type_config_factory_);
}


void PropertyGridDataSource::SetTargetObject(const std::shared_ptr<Object>& target_object) {

    ZAF_EXPECT(target_object);

    root_data_ = std::make_shared<PropertyData>(
        nullptr,
        target_object,
        false,
        type_config_factory_,
        shared_from_this());
}


bool PropertyGridDataSource::DoesDataHasChildren(const std::shared_ptr<Object>& data) {

    auto target_data = data ? As<PropertyData>(data) : root_data_;
    return !target_data->Children().empty();
}


std::size_t PropertyGridDataSource::GetChildDataCount(
    const std::shared_ptr<Object>& parent_data) {

    auto target_data = parent_data ? As<PropertyData>(parent_data) : root_data_;
    return target_data->Children().size();
}


std::shared_ptr<Object> PropertyGridDataSource::GetChildDataAtIndex(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index) {

    auto target_data = parent_data ? As<PropertyData>(parent_data) : root_data_;

    const auto& children = target_data->Children();
    ZAF_EXPECT(index < children.size());

    return children[index];
}


void PropertyGridDataSource::RefreshValues() {

    root_data_->ChangeValueFromUpToDown(root_data_->Value());
}


void PropertyGridDataSource::OnDataChildrenUpdate(
    const std::shared_ptr<PropertyData>& data,
    std::size_t children_count) {

    NotifyDataUpdate(data == root_data_ ? nullptr : data, 0, children_count);
}

}