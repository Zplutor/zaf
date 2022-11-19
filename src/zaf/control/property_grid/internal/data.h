#pragma once

#include <optional>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/control/property_grid/internal/data_observer.h>
#include <zaf/control/property_grid/property_table.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>

namespace zaf::property_grid::internal {

class Data : public Object, public std::enable_shared_from_this<Data>, NonCopyable {
public:
    Data(
        zaf::ObjectProperty* property,
        const std::shared_ptr<Object>& value,
        const std::shared_ptr<TypeConfigFactory>& type_config_factory,
        const std::weak_ptr<DataObserver>& observer);

    zaf::ObjectProperty* Property() const {
        return property_;
    }

    const std::shared_ptr<Object>& Value() const {
        return value_;
    }

    const std::vector<std::shared_ptr<Data>>& Children();

    void ResetValue(const std::shared_ptr<Object>& value);

private:
    static std::vector<ObjectType*> GetObjectTypeChain(const Object& object);
    static PropertyTable CreatePropertyTable(const std::vector<ObjectType*>& types);

private:
    std::vector<std::shared_ptr<Data>> LoadChildren() const;

private:
    zaf::ObjectProperty* property_{};
    std::shared_ptr<Object> value_{};
    std::shared_ptr<TypeConfigFactory> type_config_factory_;
    std::weak_ptr<DataObserver> observer_;

    std::optional<std::vector<std::shared_ptr<Data>>> children_;
};

}