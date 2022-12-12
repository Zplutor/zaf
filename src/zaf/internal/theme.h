#pragma once

#include <cstdint>

namespace zaf {
namespace internal {

const std::uint32_t ButtonActivedBorderColorRGB = 0x4169e1;
const std::uint32_t ButtonMouseOverBackgroundColorRGB = 0xf0f8ff;
const std::uint32_t ButtonPressedBackgroundColorRGB = 0xb0c4de;
const std::uint32_t ControlBackgroundColorRGB = 0xf0f0f0;
const std::uint32_t ControlContentColorRGB = 0xffffff;
const std::uint32_t ControlDisabledTextColorRGB = 0x838383;
const std::uint32_t ControlNormalTextColorRGB = 0x000000;
const std::uint32_t ControlSelectedActivedColorRGB = 0x3399ff;
const std::uint32_t ControlSelectedInActivedColorRGB = 0xb9b9b9;

constexpr std::uint32_t InspectedControlBorderColor = 0x32CD32;
constexpr std::uint32_t InspectedControlContentColor = 0x87CEFF;
constexpr std::uint32_t InspectedControlMarginColor = 0xF0E68C;
constexpr std::uint32_t InspectedControlPaddingColor = 0x90EE90;

}
}