#pragma once

#include <Windows.h>
#include "zaf/base/flags_enum.h"

namespace zaf {

enum class RegistryRights : REGSAM {
    None = 0,
    Read = KEY_READ,
    Write = KEY_WRITE,
};

ZAF_ENABLE_FLAGS_ENUM(RegistryRights);

}