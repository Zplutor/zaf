#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class RoutedEventSharedState {
public:
    explicit RoutedEventSharedState(const std::shared_ptr<Object>& source) : source_(source) {

    }

    const std::shared_ptr<Object>& Source() const {
        return source_;
    }

    bool IsHandled() const {
        return is_handled_;
    }

    void MarkAsHandled() {
        is_handled_ = true;
    }

private:
    std::shared_ptr<Object> source_;
    bool is_handled_{};
};


class RoutedEventInfo : public EventInfo {
public:
    RoutedEventInfo(
        const std::shared_ptr<RoutedEventSharedState>& state,
        const std::shared_ptr<Object>& sender) 
        :
        EventInfo(state->Source()),
        state_(state),
        sender_(sender) {

    }

    const std::shared_ptr<Object>& Sender() const {
        return sender_;
    }

    bool IsHandled() const {
        return state_->IsHandled();
    }

    void MarkAsHandled() const {
        state_->MarkAsHandled();
    }

private:
    std::shared_ptr<RoutedEventSharedState> state_;
    std::shared_ptr<Object> sender_;
};

}
