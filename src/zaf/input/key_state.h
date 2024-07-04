#pragma once

#include <Windows.h>
#include <zaf/base/flags_enum.h>

namespace zaf {

enum class KeyState : SHORT {
    Down = 0x80,
    Toggled = 0x1,
};

ZAF_ENABLE_FLAGS_ENUM(KeyState);

}