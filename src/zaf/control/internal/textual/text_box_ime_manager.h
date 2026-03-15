#pragma once

#include <zaf/control/event/ime_event_infos.h>
#include <zaf/control/text_box.h>

namespace zaf::internal {

class TextBoxIMEManager {
public:
    explicit TextBoxIMEManager(TextBox& owner);

    void Initialize();

    void HandleIMEStartComposition(const IMEStartCompositionInfo& event_info);
    void HandleIMEComposition(const IMECompositionInfo& event_info);

private:
    TextBox& owner_;
};

}
