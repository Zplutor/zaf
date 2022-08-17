#pragma once

#include <Windows.h>

namespace zaf {

enum class RegistryValueType {
    None = REG_NONE,
    String = REG_SZ,
    ExpandableString = REG_EXPAND_SZ,
    MultiString = REG_MULTI_SZ,
    DWord = REG_DWORD,
    QWord = REG_QWORD,
};

}