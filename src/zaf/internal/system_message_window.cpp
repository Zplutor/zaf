#include <zaf/internal/system_message_window.h>

namespace zaf::internal {

SystemMessageWindow::SystemMessageWindow() : message_only_window_(nullptr) {

    taskbar_created_message_id_ = RegisterWindowMessage(L"TaskbarCreated");

    Disposables() += message_only_window_.MessageReceivedEvent().Subscribe(
        [this](const Message& message) {
    
        if (message.ID() == taskbar_created_message_id_) {
            taskbar_created_subject_.AsObserver().OnNext(SystemTaskbarCreatedInfo{});
        }
        else if (message.ID() == WM_ENDSESSION) {
            session_ending_subject_.AsObserver().OnSuccess(SystemSessionEndingInfo{});
        }
    });
}

}