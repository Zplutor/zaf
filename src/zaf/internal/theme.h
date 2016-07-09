#pragma once

#include <cstdint>
#include <zaf/graphic/color.h>

namespace zaf {
namespace internal {

const std::uint32_t ControlBackgroundColorRGB = 0xf0f0f0;
const std::uint32_t ControlContentColorRGB = 0xffffff;
const std::uint32_t ControlDisabledTextColorRGB = 0x838383;
const std::uint32_t ControlNormalTextColorRGB = 0x000000;
extern const Color ButtonDisabledBackgroundColor;
extern const Color ButtonHoveredBackgroundColor;
extern const Color ButtonNormalBackgroundColor;
extern const Color ButtonPressedBackgroundColor;

}
}