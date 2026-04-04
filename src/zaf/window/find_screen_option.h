#pragma once

#include <Windows.h>

namespace zaf {

enum class FindScreenOption {
    DefaultToNull = MONITOR_DEFAULTTONULL,
    DefaultToPrimary = MONITOR_DEFAULTTOPRIMARY,
    DefaultToNearest = MONITOR_DEFAULTTONEAREST,
};

}