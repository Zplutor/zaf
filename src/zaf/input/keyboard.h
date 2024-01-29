#pragma once

#include <zaf/input/key.h>
#include <zaf/input/key_state.h>

namespace zaf {

/**
Provides interfaces to access the states of keys on keyboard.
*/
class Keyboard {
public:
    /**
    Gets the states for the specified key.
    */
    static KeyState GetKeyState(Key key);

    /**
    Determines whether the specified key is down.
    */
    static bool IsKeyDown(Key key);

public:
    Keyboard() = delete;
};

}