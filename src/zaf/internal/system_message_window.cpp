#include <zaf/internal/system_message_window.h>

namespace zaf::internal {

SystemMessageWindow::SystemMessageWindow() : message_only_window_(nullptr) {

    Disposables() += message_only_window_.MessageReceivedEvent().Subscribe(
        [this](const Message& message) {
    
        if (message.ID() == WM_ENDSESSION) {
            session_ended_subject_.AsObserver().OnNext(SessionEndInfo{});
        }
    });
}

}