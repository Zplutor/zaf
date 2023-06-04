#pragma once

#include <ObjIdl.h>

namespace zaf::clipboard {

enum class MediumType {
    Null = TYMED_NULL,
    GlobalMem = TYMED_HGLOBAL,
    File = TYMED_FILE,
};

}