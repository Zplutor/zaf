#pragma once

#include <zaf/internal/property_grid/property_data.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/tree_data_source.h>

namespace zaf::internal {

class PropertyGridDataSource : public TreeDataSource {
public:
    explicit PropertyGridDataSource(const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory);

    void SetTargetObject(const std::shared_ptr<Object>& target_object);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

    void RefreshValues();

private:
    std::shared_ptr<PropertyData> root_data_;
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
};

}