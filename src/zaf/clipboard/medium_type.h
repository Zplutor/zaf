#pragma once

/**
@file
    Defines the enum zaf::clipboard::MediumType.
*/

#include <ObjIdl.h>
#include <zaf/base/flags_enum.h>

namespace zaf::clipboard {

enum class MediumType {
    Null = TYMED_NULL,
    GlobalMem = TYMED_HGLOBAL,
    File = TYMED_FILE,
};

ZAF_ENABLE_FLAGS_ENUM(MediumType);

}