#pragma once

/**
@file
    Defines the `zaf::ListSelectionMode` enum.
*/

namespace zaf {

/**
Defines the selection mode of a list control that contains items.
*/
enum class ListSelectionMode {

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