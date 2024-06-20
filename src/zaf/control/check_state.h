#pragma once

#include <optional>
#include <zaf/object/enum_declaration.h>
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
ZAF_ENUM_VALUE(Unchecked)
ZAF_ENUM_VALUE(Checked)
ZAF_ENUM_VALUE(Indeterminate)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(CheckState)

}