#pragma once

#include <optional>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/internal/property_grid/property_grid_data_observer.h>
#include <zaf/control/property_grid/property_table.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class PropertyGridData : 
    public Object, 
    public std::enable_shared_from_this<PropertyGridData>, 
    SubscriptionHost,
    NonCopyableNonMovable {

public:
    PropertyGridData(
        zaf::ObjectProperty* property,
        const std::shared_ptr<Object>& value,
        bool is_parent_read_only,
        const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory,
        const std::weak_ptr<PropertyGridDataObserver>& observer);

    zaf::ObjectProperty* Property() const {
        return property_;
    }

    const std::shared_ptr<Object>& Value() const {
        return value_;
    }

    bool IsReadOnly() const {
        return is_read_only_;
    }

    const std::vector<std::shared_ptr<PropertyGridData>>& Children();

    Observable<std::shared_ptr<PropertyGridData>> ValueChangedEvent() {
        return value_changed_subject_.AsObservable();
    }

    void ChangeValueFromUpToDown(const std::shared_ptr<Object>& value);

    void ChangeValueFromDownToUp(const std::shared_ptr<Object>& value);

private:
    static std::vector<ObjectType*> GetObjectTypeChain(const Object& object);
    static property_grid::PropertyTable CreatePropertyTable(const std::vector<ObjectType*>& types);

private:
    std::vector<std::shared_ptr<PropertyGridData>> LoadChildren();
    void OnChildValueChanged(const std::shared_ptr<PropertyGridData>& child);

private:
    zaf::ObjectProperty* property_{};
    std::shared_ptr<Object> value_{};
    bool is_read_only_{};
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
    std::weak_ptr<PropertyGridDataObserver> observer_;
    Subject<std::shared_ptr<PropertyGridData>> value_changed_subject_;

    std::optional<std::vector<std::shared_ptr<PropertyGridData>>> children_;
};

}