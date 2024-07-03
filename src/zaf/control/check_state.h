#pragma once

#include <zaf/object/enum_support.h>

namespace zaf {

/**
 The check state of a control.
 */
enum class CheckState {

    /**
     The control is unchecked.
     */
    Unchecked,

    /**
     The control is checked.
     */
    Checked,

    /**
     The control's check state is indeterminate.
     */
    Indeterminate,
};

ZAF_ENUM_BEGIN(CheckState)
ZAF_ENUM_CONSTANT(Unchecked)
ZAF_ENUM_CONSTANT(Checked)
ZAF_ENUM_CONSTANT(Indeterminate)
ZAF_ENUM_END;

}