#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/control.h>
#include <zaf/control/property_grid/value_view.h>
#include <zaf/object/object_property.h>

namespace zaf::property_grid {

class TypeConfig : NonCopyable {
public:
    TypeConfig() = default;
    virtual ~TypeConfig() = default;

    virtual void FilterProperties(std::vector<ObjectProperty*>& properties) { }

    virtual std::shared_ptr<ValueView> CreateValueView();

protected:
    static void ArrangeProperties(
        std::vector<ObjectProperty*>& properties, 
        const std::vector<std::wstring>& expected_order);
};

}