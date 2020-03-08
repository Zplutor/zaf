#pragma once

#include <zaf/control/layout/layouter.h>

namespace zaf {

std::shared_ptr<Layouter> GetHorizontalArrayLayouter();
std::shared_ptr<Layouter> GetVerticalArrayLayouter();

}