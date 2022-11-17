#include <zaf/control/internal/property_grid/property_grid_tree_delegate.h>
#include <zaf/base/as.h>
#include <zaf/control/internal/property_grid/property_grid_data.h>
#include <zaf/control/internal/property_grid/property_grid_item.h>
#include <zaf/creation.h>

namespace zaf::internal {

PropertyGridTreeDelegate::PropertyGridTreeDelegate(
    const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory,
    const std::shared_ptr<PropertyGridSplitDistanceManager>& split_distance_manager)
    :
    type_config_factory_(type_config_factory),
    split_distance_manager_(split_distance_manager) {

    ZAF_EXPECT(type_config_factory_);
    ZAF_EXPECT(split_distance_manager_);
}


std::shared_ptr<TreeItem> PropertyGridTreeDelegate::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto data = As<PropertyGridData>(item_data);
    ZAF_EXPECT(data);

    auto type_config = type_config_factory_->GetConfig(data->Property()->GetValueType());
    auto value_view = type_config->CreateValueView();

    return Create<PropertyGridItem>(data, value_view, split_distance_manager_);
}

}