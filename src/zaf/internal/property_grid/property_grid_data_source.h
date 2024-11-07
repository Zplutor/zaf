#pragma once

#include <zaf/internal/property_grid/property_grid_data.h>
#include <zaf/internal/property_grid/property_grid_data_observer.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/tree_data_source.h>

namespace zaf::property_grid::internal {

class PropertyGridDataSource : 
    public TreeDataSource, 
    public PropertyGridDataObserver, 
    public std::enable_shared_from_this<PropertyGridDataSource> {

public:
    explicit PropertyGridDataSource(const std::shared_ptr<TypeConfigFactory>& type_config_factory);

    void SetTargetObject(const std::shared_ptr<Object>& target_object);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

    void RefreshValues();

    void OnDataChildrenUpdate(
        const std::shared_ptr<PropertyGridData>& data,
        std::size_t children_count) override;

private:
    std::shared_ptr<PropertyGridData> root_data_;
    std::shared_ptr<TypeConfigFactory> type_config_factory_;
};

}