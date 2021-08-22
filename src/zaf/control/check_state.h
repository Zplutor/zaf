#pragma once

#include <optional>
#include <zaf/object/enum_declaration.h>

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

ZAF_DECLARE_ENUM(CheckState)
ZAF_ENABLE_ENUM_BOXING(CheckState)

}