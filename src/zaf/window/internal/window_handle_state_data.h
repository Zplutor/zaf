#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class WindowHandleStateData : zaf::NonCopyable {
public:
    WindowHandleStateData() noexcept = default;

    WindowHandleStateData(WindowHandleStateData&&) noexcept = default;
    WindowHandleStateData& operator=(WindowHandleStateData&&) noexcept = default;

    HWND handle{};
};

}