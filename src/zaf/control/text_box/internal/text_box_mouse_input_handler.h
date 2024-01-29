#pragma once

#include <optional>
#include <zaf/base/range.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/text_box/internal/text_box_module.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class TextBoxMouseInputHandler : public TextBoxModule, SubscriptionHost {
public:
    explicit TextBoxMouseInputHandler(TextBoxModuleContext* context);

    void Initialize() override;

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

private:
    void SetCaretIndexByMouse(std::size_t index, bool begin_selection);
    void OnCaretIndexChanged();

private:
    std::optional<std::size_t> begin_selecting_index_{};
    bool is_setting_selection_range_{};
};

}