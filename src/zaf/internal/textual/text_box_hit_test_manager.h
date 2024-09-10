#pragma once

#include <zaf/graphic/dwrite/hit_test_metrics.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/internal/textual/text_model.h>

namespace zaf::internal {

/**
Provides common hit test logic interfaces.
*/
class TextBoxHitTestManager : public TextBoxModule {
public:
    class HitTestPositionResult {
    public:
        Point hit_test_position;
        dwrite::HitTestPointMetrics metrics;
    };

public:
    explicit TextBoxHitTestManager(TextBoxModuleContext* context);

    void Initialize() override;

    HitTestPositionResult HitTestAtPosition(const Point& position_in_text_box) const;
    std::size_t TextIndexFromHitTestResult(const HitTestPositionResult& hit_test_result) const;
};

}