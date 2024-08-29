#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class TextBox;

namespace textual {

class CopyingInfo : public EventInfo {
public:
    explicit CopyingInfo(std::shared_ptr<TextBox> source);

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
}