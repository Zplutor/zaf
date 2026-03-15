#include <zaf/control/internal/textual/text_box_hit_test_manager.h>
#include <zaf/control/text_box.h>

namespace zaf::internal {

TextBoxHitTestManager::TextBoxHitTestManager(TextBox& owner) : owner_(owner) {

}


void TextBoxHitTestManager::Initialize() {

}


TextBoxHitTestManager::HitTestPositionResult TextBoxHitTestManager::HitTestAtPosition(
    const Point& position_in_text_box) const {

    const auto& text_box = owner_;

    auto position_in_text_rect = position_in_text_box;
    position_in_text_rect.SubtractOffset(text_box.ContentRectInSelf().position);
    position_in_text_rect.SubtractOffset(text_box.text_rect_.position);

    HitTestPositionResult result;
    result.hit_test_position = position_in_text_rect;
    result.metrics = owner_.InnerTextLayout().HitTestPoint(position_in_text_rect);
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
