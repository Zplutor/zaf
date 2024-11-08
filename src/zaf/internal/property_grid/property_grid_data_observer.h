#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class PropertyData;

class PropertyGridDataObserver : NonCopyableNonMovable {
public:
    PropertyGridDataObserver() = default;
    virtual ~PropertyGridDataObserver() = default;

    virtual void OnDataChildrenUpdate(
        const std::shared_ptr<PropertyData>& data, 
        std::size_t children_count) = 0;
};

}