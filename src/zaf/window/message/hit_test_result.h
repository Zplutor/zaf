#pragma once

#include <Windows.h>

namespace zaf {

enum class HitTestResult {
    TitleBar = HTCAPTION,
    LeftBorder = HTLEFT,
    TopBorder = HTTOP,
    TopLeftCorner = HTTOPLEFT,
    TopRightCorner = HTTOPRIGHT,
    RightBorder = HTRIGHT,
    Border = HTBORDER,
    BottomBorder = HTBOTTOM,
    BottomLeftCorner = HTBOTTOMLEFT,
    BottomRightCorner = HTBOTTOMRIGHT,
    MinimizeButton = HTMINBUTTON,
    MaximizeButton = HTMAXBUTTON,
    CloseButton = HTCLOSE,
    HelpButton = HTHELP,
    SizeBox = HTSIZE,
    SystemMenu = HTSYSMENU,
    Menu = HTMENU,
    HorizontalScrollBar = HTHSCROLL,
    VerticalScrollBar = HTVSCROLL,
    ClientArea = HTCLIENT,
    NowHere = HTNOWHERE,
    Error = HTERROR,
};

}