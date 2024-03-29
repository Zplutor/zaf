#pragma once

#include <zaf/object/object.h>

namespace zaf {

class EventInfo {
public:
    explicit EventInfo(const std::shared_ptr<Object>& source) : source_(source) {

    }

    virtual ~EventInfo() = default;

    const std::shared_ptr<Object>& Source() const {
        return source_;
    }

private:
    std::shared_ptr<Object> source_;
};

}