#include <zaf/window/message/internal/mouse_position_conversion.h>
#include <WindowsX.h>
#include <zaf/graphic/dpi.h>

namespace zaf {
namespace internal {

Point GetMousePositionFromLPARAM(
    HWND window_handle,
    LPARAM lparam,
    bool transform_coordinate) {

    POINT position;
    position.x = GET_X_LPARAM(lparam);
    position.y = GET_Y_LPARAM(lparam);

    if (transform_coordinate) {
        ScreenToClient(window_handle, &position);
    }

    auto dpi = static_cast<float>(GetDpiForWindow(window_handle));
    return ToDIPs(Point::FromPOINT(position), dpi);
}

}
}