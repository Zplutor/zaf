#pragma once

#include <zaf/internal/property_grid/property_data.h>
#include <zaf/control/property_grid_delegate.h>
#include <zaf/control/tree_data_source.h>

namespace zaf::internal {

class PropertyGridDataManager : public TreeDataSource {
public:
    explicit PropertyGridDataManager(std::shared_ptr<PropertyGridDelegate> delegate);

    void SetTargetObject(const std::shared_ptr<Object>& target_object);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

    void RefreshValues();

private:
    std::shared_ptr<PropertyData> root_data_;
    std::shared_ptr<PropertyGridDelegate> delegate_;
};

}