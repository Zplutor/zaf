#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::property_grid::internal {

class Data;

class DataObserver : NonCopyableNonMovable {
public:
    DataObserver() = default;
    virtual ~DataObserver() = default;

    virtual void OnDataChildrenUpdate(
        const std::shared_ptr<Data>& data, 
        std::size_t children_count) = 0;
};

}