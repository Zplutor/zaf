#pragma once

#include <optional>
#include <zaf/base/range.h>
#include <zaf/control/event/double_click_info.h>
#include <zaf/control/event/mouse_cursor_changing_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/textual/active_inline_object.h>
#include <zaf/graphic/dwrite/hit_test_metrics.h>
#include <zaf/internal/textual/text_box_hit_test_manager.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/rx/disposable_host.h>

namespace zaf::internal {

class TextBoxMouseInputHandler : public TextBoxModule, rx::DisposableHost {
public:
    explicit TextBoxMouseInputHandler(TextBoxModuleContext* context);

    void Initialize() override;

    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseCursorChanging(const MouseCursorChangingInfo& event_info);
    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);
    void HandleDoubleClick(const DoubleClickInfo& event_info);

private:
    void SetSelectionRangeByMouse(std::size_t index);
    void OnSelectionChanged();

    void HandleMouseOverInlineObject(
        const TextBoxHitTestManager::HitTestPositionResult& hit_test_result,
        const MouseMessage& mouse_message);

    std::shared_ptr<textual::ActiveInlineObject> FindInlineObject(
        const TextBoxHitTestManager::HitTestPositionResult& hit_test_result) const;

private:
    std::optional<std::size_t> begin_selecting_index_{};
    bool is_setting_selection_range_{};

    std::weak_ptr<textual::ActiveInlineObject> mouse_over_object_;
};

}