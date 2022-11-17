#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/internal/property_grid/property_grid_data.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/object/object_type.h>

namespace zaf::internal {

class PropertyGridTreeDataSource : public TreeDataSource {
public:
    explicit PropertyGridTreeDataSource(
        const std::shared_ptr<Object>& target_object,
        const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

private:
    std::shared_ptr<PropertyGridData> CreateData(
        ObjectProperty* property,
        const std::shared_ptr<Object>& value) const; 

    std::vector<ObjectProperty*> InspectProperties(const std::shared_ptr<Object>& value) const;

private:
    std::shared_ptr<PropertyGridData> root_data_;
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
};

}