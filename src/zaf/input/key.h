#pragma once

#include <Windows.h>

namespace zaf {

/**
Defines virtual keys on a keyboard.
*/
enum class Key : int {

    A = L'A',
    B = L'B',
    C = L'C',
    D = L'D',
    E = L'E',
    F = L'F',
    G = L'G',
    H = L'H',
    I = L'I',
    J = L'J',
    K = L'K',
    L = L'L',
    M = L'M',
    N = L'N',
    O = L'O',
    P = L'P',
    Q = L'Q',
    R = L'R',
    S = L'S',
    T = L'T',
    U = L'U',
    V = L'V',
    W = L'W',
    X = L'X',
    Y = L'Y',
    Z = L'Z',

    Alt = VK_MENU,
    Backspace = VK_BACK,
    Ctrl = VK_CONTROL,
    Delete = VK_DELETE,
    Down = VK_DOWN,
    End = VK_END,
    Enter = VK_RETURN,
    Escape = VK_ESCAPE,
    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,
    Home = VK_HOME,
    Left = VK_LEFT,
    Right = VK_RIGHT,
    Shift = VK_SHIFT,
    Space = VK_SPACE,
    Tab = VK_TAB,
    Up = VK_UP,
};

}