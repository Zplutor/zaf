#pragma once

#include <Windows.h>
#include <zaf/base/flag_enum.h>

namespace zaf {

enum class KeyState : SHORT {
    Down = 0x80,
    Toggled = 0x1,
};

ZAF_ENABLE_FLAG_ENUM(KeyState);

}