#pragma once

#include <optional>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/control/property_grid/property_table.h>
#include <zaf/control/property_grid_delegate.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/disposable_host.h>

namespace zaf::internal {

class PropertyData : 
    public Object, 
    public std::enable_shared_from_this<PropertyData>, 
    rx::DisposableHost,
    NonCopyableNonMovable {

public:
    PropertyData(
        std::shared_ptr<PropertyData> parent,
        zaf::ObjectProperty* property,
        const std::shared_ptr<Object>& value,
        const std::shared_ptr<PropertyGridDelegate>& delegate);

    zaf::ObjectProperty* Property() const {
        return property_;
    }

    const std::shared_ptr<Object>& Value() const {
        return value_;
    }

    bool IsReadOnly() const;

    const std::vector<std::shared_ptr<PropertyData>>& Children();

    void ChangeValue(std::shared_ptr<Object> value);

    rx::Observable<std::shared_ptr<PropertyData>> ValueChangedEvent() {
        return value_changed_event_.AsObservable();
    }

private:
    static std::vector<ObjectType*> GetObjectTypeChain(const Object& object);
    static property_grid::PropertyTable CreatePropertyTable(const std::vector<ObjectType*>& types);

private:
    std::vector<std::shared_ptr<PropertyData>> LoadChildren();

    void SetValue(std::shared_ptr<Object> new_value);
    void PropagateValueChangedToParent();
    void ChangeValueFromChild(const PropertyData& child);

    void PropagateValueChangedToChildren();
    void ChangeValueFromParent(std::shared_ptr<Object> new_value);

private:
    std::weak_ptr<PropertyData> parent_;

    zaf::ObjectProperty* property_{};
    std::shared_ptr<Object> value_{};

    std::shared_ptr<PropertyGridDelegate> delegate_;

    bool is_changing_value_{};
    rx::Subject<std::shared_ptr<PropertyData>> value_changed_event_;

    std::optional<std::vector<std::shared_ptr<PropertyData>>> children_;
};

}