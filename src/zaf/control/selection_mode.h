#pragma once

namespace zaf {

/**
 Defines the selection mode of a control that contains items.
 */
enum class SelectionMode {

    /**
     The list control items can not be selected.
     */
    None,

    /**
     Only one single list control item can be selected.
     */
    Single,

    /**
     Multiple list control items can be selected with mouse clicking.
     */
    SimpleMultiple,

    /**
     Multiple list control items can be selected with the help of SHIFT and CTRL keys.
     */
    ExtendedMultiple,
};

}