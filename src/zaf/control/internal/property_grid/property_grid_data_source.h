#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/internal/property_grid/property_grid_data.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/object/object_type.h>

namespace zaf::internal {

class PropertyGridDataSource : public TreeDataSource {
public:
    explicit PropertyGridDataSource(const std::shared_ptr<Object>& target_object);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

private:
    std::shared_ptr<PropertyGridData> root_data_;
};

}