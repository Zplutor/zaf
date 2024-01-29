#include <zaf/input/keyboard.h>

namespace zaf {

KeyState Keyboard::GetKeyState(Key key) {
    return static_cast<KeyState>(::GetKeyState(static_cast<int>(key)));
}


bool Keyboard::IsKeyDown(Key key) {
    return zaf::HasFlag(GetKeyState(key), KeyState::Down);
}


bool Keyboard::IsShiftDown(){
    return IsKeyDown(Key::Shift);
}


bool Keyboard::IsCtrlDown(){
    return IsKeyDown(Key::Ctrl);
}


bool Keyboard::IsAltDown(){
    return IsKeyDown(Key::Alt);
}

}