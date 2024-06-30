#include <zaf/control/property_grid/internal/delegate.h>
#include <zaf/base/as.h>
#include <zaf/control/property_grid/internal/data.h>
#include <zaf/control/property_grid/internal/item.h>
#include <zaf/creation.h>

namespace zaf::property_grid::internal {

Delegate::Delegate(
    const std::shared_ptr<TypeConfigFactory>& type_config_factory,
    const std::shared_ptr<SplitDistanceManager>& split_distance_manager,
    const std::weak_ptr<zaf::internal::TreeControlImplementation>& tree_implementation)
    :
    type_config_factory_(type_config_factory),
    split_distance_manager_(split_distance_manager),
    tree_implementation_(tree_implementation) {

    ZAF_EXPECT(type_config_factory_);
    ZAF_EXPECT(split_distance_manager_);
}


std::shared_ptr<TreeItem> Delegate::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto data = As<Data>(item_data);
    ZAF_EXPECT(data);

    auto type_config = type_config_factory_->GetConfig(data->Property()->ValueType());
    auto value_view = type_config->CreateValueView();

    std::weak_ptr<Object> weak_data = data;
    Subscriptions() += value_view->ShouldSelectEvent().Subscribe(
        std::bind(&Delegate::OnValueViewShouldSelect, this, weak_data));

    return Create<Item>(data, value_view, split_distance_manager_);
}


void Delegate::OnValueViewShouldSelect(const std::weak_ptr<Object>& weak_data) {

    auto data = weak_data.lock();
    if (!data) {
        return;
    }

    auto tree_implementation = tree_implementation_.lock();
    if (!tree_implementation) {
        return;
    }

    tree_implementation->SelectItem(data);
}

}