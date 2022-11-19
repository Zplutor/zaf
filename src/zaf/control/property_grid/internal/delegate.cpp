#include <zaf/control/property_grid/internal/delegate.h>
#include <zaf/base/as.h>
#include <zaf/control/property_grid/internal/data.h>
#include <zaf/control/property_grid/internal/item.h>
#include <zaf/creation.h>

namespace zaf::property_grid::internal {

Delegate::Delegate(
    const std::shared_ptr<TypeConfigFactory>& type_config_factory,
    const std::shared_ptr<SplitDistanceManager>& split_distance_manager)
    :
    type_config_factory_(type_config_factory),
    split_distance_manager_(split_distance_manager) {

    ZAF_EXPECT(type_config_factory_);
    ZAF_EXPECT(split_distance_manager_);
}


std::shared_ptr<TreeItem> Delegate::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto data = As<Data>(item_data);
    ZAF_EXPECT(data);

    auto type_config = type_config_factory_->GetConfig(data->Property()->GetValueType());
    auto value_view = type_config->CreateValueView();

    return Create<Item>(data, value_view, split_distance_manager_);
}

}