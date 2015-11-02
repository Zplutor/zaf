#pragma once

#include <zaf/internal/layouter.h>

namespace zaf {
namespace internal {

class AnchorLayouter : public Layouter {
public:
	void Layout(const Control& control, const Rect& previous_rect) override;
};

}
}