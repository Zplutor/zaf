#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/graphic/text/text_range.h>
#include <zaf/window/message/message.h>

namespace zaf {

class RichEdit;

enum class TextChangeReason {
    Unknown,
    KeyInput,
    Paste,
};

class TextChangingInfo : public EventInfo {
public:
    TextChangingInfo(
        const std::shared_ptr<RichEdit>& source,
        TextChangeReason reason,
        const zaf::TextRange& text_range,
        const Message& triggered_message);

    TextChangeReason Reason() const {
        return reason_;
    }

    const zaf::TextRange& TextRange() const {
        return text_range_;
    }

    const Message& TriggeredMessage() const {
        return triggered_message_;
    }

    bool CanChange() const {
        return *can_change_;
    }

    void SetCanChange(bool can_change) const {
        *can_change_ = can_change;
    }

private:
    TextChangeReason reason_{ TextChangeReason::Unknown };
    zaf::TextRange text_range_;
    Message triggered_message_;
    std::shared_ptr<bool> can_change_{};
};

}