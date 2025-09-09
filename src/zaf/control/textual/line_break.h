#pragma once

/**
@file
    Defines the zaf::textual::LineBreak enum.
*/

#include <zaf/dynamic/enum_support.h>

namespace zaf::textual {

/**
Represents the line break style used in a textual control.
*/
enum class LineBreak {

    /**
    The line break style is unspecific.
    */
    Unspecific,

    /**
    The CRLF(`\r\n`) line break.
    */
    CRLF,

    /**
    The CR(`\r`) line break.
    */
    CR,

    /**
    The LF(`\n`) line break.
    */
    LF,
};

ZAF_ENUM_BEGIN(LineBreak);
ZAF_ENUM_CONSTANT(Unspecific);
ZAF_ENUM_CONSTANT(CRLF);
ZAF_ENUM_CONSTANT(CR);
ZAF_ENUM_CONSTANT(LF);
ZAF_ENUM_END;

}