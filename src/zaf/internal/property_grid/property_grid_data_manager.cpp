#include <zaf/internal/property_grid/property_grid_data_manager.h>
#include <zaf/base/as.h>

namespace zaf::internal {

PropertyGridDataManager::PropertyGridDataManager(std::shared_ptr<PropertyGridDelegate> delegate) :
    delegate_(delegate) {

    ZAF_EXPECT(delegate_);
}


void PropertyGridDataManager::SetTargetObject(const std::shared_ptr<dynamic::Object>& target_object) {

    ZAF_EXPECT(target_object);

    root_data_ = std::make_shared<PropertyData>(
        nullptr,
        nullptr,
        target_object,
        delegate_);
}


bool PropertyGridDataManager::DoesDataHasChildren(const std::shared_ptr<dynamic::Object>& data) {

    auto target_data = data ? As<PropertyData>(data) : root_data_;
    return !target_data->Children().empty();
}


std::size_t PropertyGridDataManager::GetChildDataCount(
    const std::shared_ptr<dynamic::Object>& parent_data) {

    auto target_data = parent_data ? As<PropertyData>(parent_data) : root_data_;
    return target_data->Children().size();
}


std::shared_ptr<dynamic::Object> PropertyGridDataManager::GetChildDataAtIndex(
    const std::shared_ptr<dynamic::Object>& parent_data,
    std::size_t index) {

    auto target_data = parent_data ? As<PropertyData>(parent_data) : root_data_;

    const auto& children = target_data->Children();
    ZAF_EXPECT(index < children.size());

    return children[index];
}


void PropertyGridDataManager::RefreshValues() {

    root_data_->ChangeValue(root_data_->Value());
}

}