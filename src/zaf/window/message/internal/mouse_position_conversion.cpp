#include <zaf/window/message/internal/mouse_position_conversion.h>
#include <WindowsX.h>

namespace zaf {
namespace internal {

Point GetMousePositionFromLPARAM(LPARAM lparam, HWND transformed_window_handle) {

    POINT position;
    position.x = GET_X_LPARAM(lparam);
    position.y = GET_Y_LPARAM(lparam);

    if (transformed_window_handle != nullptr) {
        ScreenToClient(transformed_window_handle, &position);
    }

    return Point::FromPOINT(position);
}

}
}