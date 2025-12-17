#pragma once

#include <optional>
#include <zaf/base/none.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject/single_subject.h>

namespace zaf {
class Control;
class TooltipWindow;
}

namespace zaf::internal {

enum class TrackMouseMode {
    None,
    ClientArea,
    NonClientArea,
};

class WindowHandleStateData : zaf::NonCopyable {
public:
    WindowHandleStateData() noexcept = default;

    WindowHandleStateData(WindowHandleStateData&&) noexcept = default;
    WindowHandleStateData& operator=(WindowHandleStateData&&) noexcept = default;

    HWND handle{};

    bool is_sizing_or_moving{};
    std::optional<rx::SingleSubject<zaf::None>> exit_sizing_or_moving_subject;

    TrackMouseMode track_mouse_mode{ TrackMouseMode::None };
    std::shared_ptr<Control> mouse_over_control;
    std::shared_ptr<Control> mouse_capture_control;

    std::shared_ptr<TooltipWindow> tooltip_window;
};

}