#pragma once

#include <zaf/base/event/routed_event_info.h>
#include <zaf/window/message/ime_message.h>

namespace zaf {

class Control;

namespace internal {

class IMEEventSharedState : public RoutedEventSharedState {
public:
    IMEEventSharedState(std::shared_ptr<Control> source, const zaf::Message& message) noexcept;

    const zaf::Message& Message() const noexcept {
        return message_;
    }

private:
    zaf::Message message_;
};

}

class IMEStartCompositionInfo : public RoutedEventInfo {
public:
    IMEStartCompositionInfo(
        std::shared_ptr<internal::IMEEventSharedState> state, 
        std::shared_ptr<Control> sender) noexcept;

    const zaf::Message& Message() const noexcept {
        return shared_state_->Message();
    }

private:
    std::shared_ptr<internal::IMEEventSharedState> shared_state_;
};


class IMECompositionInfo : public RoutedEventInfo {
public:
    IMECompositionInfo(
        std::shared_ptr<internal::IMEEventSharedState> state, 
        std::shared_ptr<Control> sender) noexcept;

    IMECompositionMessage Message() const noexcept {
        return IMECompositionMessage{ shared_state_->Message() };
    }

private:
    std::shared_ptr<internal::IMEEventSharedState> shared_state_;
};


class IMEEndCompositionInfo : public RoutedEventInfo {
public:
    IMEEndCompositionInfo(
        std::shared_ptr<internal::IMEEventSharedState> state,
        std::shared_ptr<Control> sender) noexcept;

    const zaf::Message& Message() const noexcept {
        return shared_state_->Message();
    }

private:
    std::shared_ptr<internal::IMEEventSharedState> shared_state_;
};

}