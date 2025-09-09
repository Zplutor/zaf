#pragma once

#include <zaf/dynamic/object.h>

namespace zaf {

class EventInfo {
public:
    explicit EventInfo(const std::shared_ptr<dynamic::Object>& source) : source_(source) {

    }

    virtual ~EventInfo() = default;

    const std::shared_ptr<dynamic::Object>& Source() const {
        return source_;
    }

private:
    std::shared_ptr<dynamic::Object> source_;
};

}