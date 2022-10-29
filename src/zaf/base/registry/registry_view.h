#pragma once

#include <Windows.h>

namespace zaf {

enum class RegistryView {
    Default = 0,
    Registry32 = KEY_WOW64_32KEY,
    Registry64 = KEY_WOW64_64KEY,
};

}