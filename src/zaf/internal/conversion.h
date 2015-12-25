#pragma once

#include <Windows.h>
#include <zaf/internal/direct2d.h>

namespace zaf {

class Rect;

D2D1_RECT_F ToD2dRect(const Rect& rect);

}