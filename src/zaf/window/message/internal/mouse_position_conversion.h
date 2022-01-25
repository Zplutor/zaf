#pragma once

#include <zaf/graphic/point.h>

namespace zaf {
namespace internal {

Point GetMousePositionFromLPARAM(
    HWND window_handle, 
    LPARAM lparam,
    bool transform_coordinate);

}
}