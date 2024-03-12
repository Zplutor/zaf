#pragma once

#include <optional>
#include <zaf/base/range.h>
#include <zaf/control/event/mouse_cursor_changing_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/graphic/text/hit_test_result.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class TextBoxMouseInputHandler : public TextBoxModule, SubscriptionHost {
public:
    explicit TextBoxMouseInputHandler(TextBoxModuleContext* context);

    void Initialize() override;

    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseCursorChanging(const MouseCursorChangingInfo& event_info);
    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

private:
    void SetCaretIndexByMouse(std::size_t index, bool begin_selection);
    void OnSelectionChanged();

    void HandleMouseOverInlineObject(
        const HitTestPointResult& hit_test_result,
        const MouseMessage& mouse_message);

    std::shared_ptr<textual::DynamicInlineObject> FindInlineObject(
        const HitTestPointResult& hit_test_result) const;

private:
    std::optional<std::size_t> begin_selecting_index_{};
    bool is_setting_selection_range_{};

    std::weak_ptr<textual::DynamicInlineObject> mouse_over_object_;
};

}