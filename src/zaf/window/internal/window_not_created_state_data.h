#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/size.h>
#include <zaf/window/internal/window_styles.h>

namespace zaf::internal {

class WindowNotCreatedStateData : zaf::NonCopyable {
public:
    WindowNotCreatedStateData() noexcept = default;

    WindowNotCreatedStateData(WindowNotCreatedStateData&&) noexcept = default;
    WindowNotCreatedStateData& operator=(WindowNotCreatedStateData&&) noexcept = default;

    std::wstring title;
    WindowBasicStyle basic_style = WindowBasicStyle::Default();
    WindowExtendedStyle extended_style;
    std::optional<Point> initial_position;
    Size size{ 640, 480 };
};

}