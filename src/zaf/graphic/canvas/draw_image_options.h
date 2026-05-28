#pragma once

/**
@file
    Defines the `zaf::DrawImageOptions` class.
*/

#include <optional>
#include <zaf/graphic/d2d/interpolation_mode.h>
#include <zaf/graphic/rect.h>

namespace zaf {

/**
Contains options for drawing image.
*/
class DrawImageOptions {
public:
    float opacity{ 1.f };
    d2d::InterpolationMode interpolation_mode{ d2d::InterpolationMode::Linear };
    std::optional<Rect> source_rect;
};

}