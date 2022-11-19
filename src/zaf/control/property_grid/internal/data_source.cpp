#include <zaf/control/property_grid/internal/data_source.h>
#include <zaf/base/as.h>

namespace zaf::property_grid::internal {

DataSource::DataSource(const std::shared_ptr<TypeConfigFactory>& type_config_factory) :
    type_config_factory_(type_config_factory) {

    ZAF_EXPECT(type_config_factory_);
}


void DataSource::SetTargetObject(const std::shared_ptr<Object>& target_object) {

    ZAF_EXPECT(target_object);

    root_data_ = std::make_shared<Data>(
        nullptr,
        target_object,
        type_config_factory_,
        shared_from_this());
}


bool DataSource::DoesDataHasChildren(const std::shared_ptr<Object>& data) {

    auto target_data = data ? As<Data>(data) : root_data_;
    return !target_data->Children().empty();
}


std::size_t DataSource::GetChildDataCount(
    const std::shared_ptr<Object>& parent_data) {

    auto target_data = parent_data ? As<Data>(parent_data) : root_data_;
    return target_data->Children().size();
}


std::shared_ptr<Object> DataSource::GetChildDataAtIndex(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index) {

    auto target_data = parent_data ? As<Data>(parent_data) : root_data_;

    const auto& children = target_data->Children();
    ZAF_EXPECT(index < children.size());

    return children[index];
}


void DataSource::RefreshValues() {

    root_data_->ResetValue(root_data_->Value());
}


void DataSource::OnDataChildrenUpdate(
    const std::shared_ptr<Data>& data,
    std::size_t children_count) {

    NotifyDataUpdate(data, 0, children_count);
}

}