#pragma once

#include <zaf/graphic/text/hit_test_metrics.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/internal/textual/text_model.h>

namespace zaf::internal {

/**
Provides common hit test logic interfaces.
*/
class TextBoxHitTestManager : public TextBoxModule {
public:
    explicit TextBoxHitTestManager(TextBoxModuleContext* context);

    void Initialize() override;

    HitTestPointMetrics HitTestAtPosition(const Point& position_in_text_box) const;
    std::size_t TextIndexFromHitTestResult(const HitTestPointMetrics& hit_test_result) const;
};

}