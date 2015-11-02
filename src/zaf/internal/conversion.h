#pragma once

#include <Windows.h>
#include <zaf/internal/direct2d.h>

namespace zaf {

class Point;
class Rect;

RECT ToWin32Rect(const Rect& rect);
Rect FromWin32Rect(const RECT& win32_rect);

POINT ToWin32Point(const Point& point);
Point FromWin32Point(const POINT& win32_point);

D2D1_RECT_F ToD2dRect(const Rect& rect);

}