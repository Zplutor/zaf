#include <zaf/window/event/message_handling_info.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/window/window.h>

namespace zaf {

MessageHandlingInfo::MessageHandlingInfo(
    const std::shared_ptr<Window>& source,
    const zaf::Message& message,
    bool can_be_handled)
    :
    EventInfo(source),
    state_(std::make_shared<internal::MessageHandlingSharedState>(message, can_be_handled)) {

}


void MessageHandlingInfo::MarkAsHandled(LRESULT handle_result) const {

    if (!this->CanBeHandled()) {
        throw zaf::InvalidOperationError(ZAF_SOURCE_LOCATION());
    }

    state_->handle_result = handle_result;
}

}