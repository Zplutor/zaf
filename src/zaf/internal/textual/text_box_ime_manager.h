#pragma once

#include <zaf/control/event/ime_event_infos.h>
#include <zaf/internal/textual/text_box_module.h>

namespace zaf::internal {

class TextBoxIMEManager : public TextBoxModule {
public:
    explicit TextBoxIMEManager(TextBoxModuleContext* context);

    void Initialize() override;

    void HandleIMEStartComposition(const IMEStartCompositionInfo& event_info);
    void HandleIMEComposition(const IMECompositionInfo& event_info);
};

}