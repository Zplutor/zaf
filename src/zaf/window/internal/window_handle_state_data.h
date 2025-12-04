#pragma once

#include <optional>
#include <zaf/base/none.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject/single_subject.h>

namespace zaf::internal {

class WindowHandleStateData : zaf::NonCopyable {
public:
    WindowHandleStateData() noexcept = default;

    WindowHandleStateData(WindowHandleStateData&&) noexcept = default;
    WindowHandleStateData& operator=(WindowHandleStateData&&) noexcept = default;

    HWND handle{};

    bool is_sizing_or_moving{};
    std::optional<rx::SingleSubject<zaf::None>> exit_sizing_or_moving_subject;
};

}