#pragma once

#include <zaf/graphic/point.h>

namespace zaf {
namespace internal {

Point GetMousePositionFromLPARAM(LPARAM lparam, HWND transformed_window_handle);

}
}