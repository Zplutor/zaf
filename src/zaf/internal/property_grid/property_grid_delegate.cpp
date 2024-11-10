#include <zaf/internal/property_grid/property_grid_delegate.h>
#include <zaf/base/as.h>
#include <zaf/internal/property_grid/property_data.h>
#include <zaf/internal/property_grid/property_grid_item.h>
#include <zaf/creation.h>

namespace zaf::internal {

PropertyGridDelegate::PropertyGridDelegate(
    const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory,
    const std::shared_ptr<SplitDistanceManager>& split_distance_manager,
    const std::weak_ptr<zaf::internal::TreeControlImplementation>& tree_implementation)
    :
    type_config_factory_(type_config_factory),
    split_distance_manager_(split_distance_manager),
    tree_implementation_(tree_implementation) {

    ZAF_EXPECT(type_config_factory_);
    ZAF_EXPECT(split_distance_manager_);
}


std::shared_ptr<TreeItem> PropertyGridDelegate::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto data = As<PropertyData>(item_data);
    ZAF_EXPECT(data);

    auto type_config = type_config_factory_->GetConfig(data->Property()->ValueType());
    auto value_editor = type_config->CreateValueEditor();

    std::weak_ptr<Object> weak_data = data;
    Subscriptions() += value_editor->FocusGainedEvent().Subscribe(
        std::bind_front(&PropertyGridDelegate::OnValueEditorGainedFocus, this, weak_data));

    return Create<PropertyGridItem>(data, value_editor, split_distance_manager_);
}


void PropertyGridDelegate::OnValueEditorGainedFocus(
    const std::weak_ptr<Object>& weak_data,
    const FocusGainedInfo& event_info) {

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