#pragma once

#include <zaf/control/property_grid/internal/data.h>
#include <zaf/control/property_grid/internal/data_observer.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/tree_data_source.h>

namespace zaf::property_grid::internal {

class DataSource : 
    public TreeDataSource, 
    public DataObserver, 
    public std::enable_shared_from_this<DataSource> {

public:
    explicit DataSource(const std::shared_ptr<TypeConfigFactory>& type_config_factory);

    void SetTargetObject(const std::shared_ptr<Object>& target_object);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

    void RefreshValues();

    void OnDataChildrenUpdate(
        const std::shared_ptr<Data>& data,
        std::size_t children_count) override;

private:
    std::shared_ptr<Data> root_data_;
    std::shared_ptr<TypeConfigFactory> type_config_factory_;
};

}