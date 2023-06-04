#pragma once

#include <ObjIdl.h>

namespace zaf {

enum class DataMediumType {
    Null = TYMED_NULL,
    GlobalMem = TYMED_HGLOBAL,
    File = TYMED_FILE,
};

}