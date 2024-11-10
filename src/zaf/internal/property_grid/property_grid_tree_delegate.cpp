#include <zaf/internal/property_grid/property_grid_tree_delegate.h>
#include <zaf/base/as.h>
#include <zaf/creation.h>
#include <zaf/internal/property_grid/property_data.h>
#include <zaf/internal/property_grid/property_grid_item.h>

namespace zaf::internal {

PropertyGridTreeDelegate::PropertyGridTreeDelegate(
    const std::shared_ptr<PropertyGridDelegate>& delegate,
    const std::shared_ptr<SplitDistanceManager>& split_distance_manager,
    const std::weak_ptr<zaf::internal::TreeControlImplementation>& tree_implementation)
    :
    delegate_(delegate),
    split_distance_manager_(split_distance_manager),
    tree_implementation_(tree_implementation) {

    ZAF_EXPECT(delegate_);
    ZAF_EXPECT(split_distance_manager_);
}


std::shared_ptr<TreeItem> PropertyGridTreeDelegate::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto data = As<PropertyData>(item_data);
    ZAF_EXPECT(data);

    auto type_config = delegate_->GetTypeConfig(data->Property()->ValueType());
    auto value_editor = type_config->CreateValueEditor();

    std::weak_ptr<Object> weak_data = data;
    Subscriptions() += value_editor->FocusGainedEvent().Subscribe(
        std::bind_front(&PropertyGridTreeDelegate::OnValueEditorGainedFocus, this, weak_data));

    return Create<PropertyGridItem>(data, value_editor, split_distance_manager_);
}


void PropertyGridTreeDelegate::OnValueEditorGainedFocus(
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