#pragma once

#include <zaf/base/non_copyable.h>
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
};

}