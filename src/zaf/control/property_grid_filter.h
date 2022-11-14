#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>

namespace zaf {

class PropertyGridFilter : NonCopyable {
public:
    PropertyGridFilter() = default;
    virtual ~PropertyGridFilter() = default;

    virtual void FilterProperties(
        const std::shared_ptr<Object>& value,
        std::vector<ObjectProperty*>& properties) {
    
    }
};

}