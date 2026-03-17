#include <zaf/control/event/ime_event_infos.h>
#include <zaf/control/control.h>

namespace zaf {
namespace internal {

IMEEventSharedState::IMEEventSharedState(
    std::shared_ptr<Control> source, 
    const zaf::Message& message) noexcept
    :
    RoutedEventSharedState(std::move(source)),
    message_(message) {

}

}

IMEStartCompositionInfo::IMEStartCompositionInfo(
    std::shared_ptr<internal::IMEEventSharedState> state,
    std::shared_ptr<Control> sender) noexcept
    :
    RoutedEventInfo(state, std::move(sender)),
    shared_state_(state) {

}


IMECompositionInfo::IMECompositionInfo(
    std::shared_ptr<internal::IMEEventSharedState> state,
    std::shared_ptr<Control> sender) noexcept
    : 
    RoutedEventInfo(state, std::move(sender)),
    shared_state_(state) {

}


IMEEndCompositionInfo::IMEEndCompositionInfo(
    std::shared_ptr<internal::IMEEventSharedState> state,
    std::shared_ptr<Control> sender) noexcept
    :
    RoutedEventInfo(state, std::move(sender)),
    shared_state_(state) {

}

}
