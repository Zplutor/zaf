#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/rx/subject/single_subject.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/window/internal/inner_message_only_window.h>

namespace zaf::internal {

class SystemMessageWindow : public rx::DisposableHost {
public:
    SystemMessageWindow();

    rx::Observable<SystemTaskbarCreatedInfo> TaskbarCreatedEvent() const noexcept {
        return taskbar_created_subject_.AsObservable();
    }

    rx::Single<SystemSessionEndingInfo> SessionEndingEvent() const noexcept {
        return session_ending_subject_.AsSingle();
    }

private:
    InnerMessageOnlyWindow message_only_window_;
    UINT taskbar_created_message_id_{};
    rx::Subject<SystemTaskbarCreatedInfo> taskbar_created_subject_;
    rx::SingleSubject<SystemSessionEndingInfo> session_ending_subject_;
};

}