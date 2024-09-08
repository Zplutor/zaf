#include <zaf/internal/textual/text_box_hit_test_manager.h>
#include <zaf/internal/textual/text_box_module_context.h>

namespace zaf::internal {

TextBoxHitTestManager::TextBoxHitTestManager(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxHitTestManager::Initialize() {

}


TextBoxHitTestManager::HitTestPositionResult TextBoxHitTestManager::HitTestAtPosition(
    const Point& position_in_text_box) const {

    const auto& text_box = Context().Owner();

    auto position_in_text_rect = position_in_text_box;
    position_in_text_rect.SubtractOffset(text_box.ContentRect().position);
    position_in_text_rect.SubtractOffset(text_box.text_rect_.position);

    HitTestPositionResult result;
    result.hit_test_position = position_in_text_rect;
    result.metrics = Context().GetTextLayout().HitTestPoint(position_in_text_rect);
    return result;
}


std::size_t TextBoxHitTestManager::TextIndexFromHitTestResult(
    const HitTestPositionResult& hit_test_result) const {

    std::size_t result = hit_test_result.metrics.Metrics().TextIndex();
    if (hit_test_result.metrics.IsTrailingHit()) {
        ++result;
    }
    return result;
}

}