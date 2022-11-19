#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/property_grid/internal/data.h>
#include <zaf/control/property_grid/property_table.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/object/object_type.h>

namespace zaf::property_grid::internal {

class DataSource : public TreeDataSource {
public:
    explicit DataSource(
        const std::shared_ptr<Object>& target_object,
        const std::shared_ptr<TypeConfigFactory>& type_config_factory);

    bool DoesDataHasChildren(const std::shared_ptr<Object>& data);

    std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data);

    std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index);

private:
    static std::vector<ObjectType*> GetObjectTypeChain(const Object& object);
    static PropertyTable CreatePropertyTable(const std::vector<ObjectType*>& types);

private:
    std::shared_ptr<Data> CreateData(
        ObjectProperty* property,
        const std::shared_ptr<Object>& value) const; 

    std::vector<ObjectProperty*> InspectProperties(const std::shared_ptr<Object>& value) const;

private:
    std::shared_ptr<Data> root_data_;
    std::shared_ptr<TypeConfigFactory> type_config_factory_;
};

}