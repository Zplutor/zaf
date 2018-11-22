#pragma once

#include <Windows.h>
#include "zaf/base/flag_enum.h"

namespace zaf {

enum class RegistryRights : REGSAM {
    Read = KEY_READ,
    Write = KEY_WRITE,
};

ZAF_ENABLE_FLAG_ENUM(RegistryRights);

}