#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class WindowNotCreatedStateData : zaf::NonCopyable {
public:
    WindowNotCreatedStateData() noexcept = default;

    WindowNotCreatedStateData(WindowNotCreatedStateData&&) noexcept = default;
    WindowNotCreatedStateData& operator=(WindowNotCreatedStateData&&) noexcept = default;

    std::wstring title;
};

}