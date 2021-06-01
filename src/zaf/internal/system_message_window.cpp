#include <zaf/internal/system_message_window.h>

namespace zaf::internal {

SystemMessageWindow::SystemMessageWindow() : message_only_window_(nullptr) {

	Subscriptions() += message_only_window_.ReceiveMessageEvent().Subscribe(
		[this](const Message& message) {
	
		if (message.id == WM_ENDSESSION) {
			session_ended_subject_.GetObserver().OnNext(SessionEndedInfo{});
		}
	});
}

}