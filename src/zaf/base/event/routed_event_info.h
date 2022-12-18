#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class RoutedEventInfo : public EventInfo {
public:
    explicit RoutedEventInfo(const std::shared_ptr<Object>& source) :
        EventInfo(source),
        is_handled_(std::make_shared<bool>()) {

    }

    bool IsHandled() const {
        return *is_handled_;
    }

    void MarkAsHandled() const {
        *is_handled_ = true;
    }

private:
    std::shared_ptr<bool> is_handled_;
};

}
