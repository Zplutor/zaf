#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/rx/subject/single_subject.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/window/internal/inner_message_only_window.h>

namespace zaf::internal {

class SystemMessageWindow : public rx::DisposableHost {
public:
    SystemMessageWindow();

    rx::Single<SystemSessionEndingInfo> SessionEndingEvent() const noexcept {
        return session_ending_subject_.AsSingle();
    }

private:
    InnerMessageOnlyWindow message_only_window_;
    rx::SingleSubject<SystemSessionEndingInfo> session_ending_subject_;
};

}