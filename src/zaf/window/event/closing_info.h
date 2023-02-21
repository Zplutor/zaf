#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class ClosingInfo : public EventInfo {
public:
    explicit ClosingInfo(const std::shared_ptr<Window>& source);

    bool CanClose() const {
        return *can_close_;
    }

    void SetCanClose(bool can_close) const {
        *can_close_ = can_close;
    }

private:
    std::shared_ptr<bool> can_close_;
};

}