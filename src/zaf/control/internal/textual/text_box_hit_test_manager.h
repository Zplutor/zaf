#pragma once

#include <zaf/control/text_box.h>
#include <zaf/graphic/dwrite/hit_test_metrics.h>
#include <zaf/control/internal/textual/text_model.h>

namespace zaf::internal {

/**
Provides common hit test logic interfaces.
*/
class TextBoxHitTestManager {
public:
    class HitTestPositionResult {
    public:
        Point hit_test_position;
        dwrite::HitTestPointMetrics metrics;
    };

public:
    explicit TextBoxHitTestManager(TextBox& owner);

    void Initialize();

    HitTestPositionResult HitTestAtPosition(const Point& position_in_text_box) const;
    std::size_t TextIndexFromHitTestResult(const HitTestPositionResult& hit_test_result) const;

private:
    TextBox& owner_;
};

}
