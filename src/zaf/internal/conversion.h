#pragma once

#include <Windows.h>
#include <zaf/internal/direct2d.h>

namespace zaf {

class Point;
class Rect;

POINT ToWin32Point(const Point& point);
Point FromWin32Point(const POINT& win32_point);

D2D1_RECT_F ToD2dRect(const Rect& rect);

}