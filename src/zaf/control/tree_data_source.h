#pragma once

#include <zaf/base/define.h>
#include <zaf/object/object.h>

namespace zaf {

class TreeDataSource {
public:
    virtual bool DoesDataHasChildren(const std::shared_ptr<Object>& data) {
        return true;
    }

    virtual std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data) {
        return 0;
    }

    virtual std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index) {

        return Object::Empty();
    }
};

}