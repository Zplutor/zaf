#pragma once

namespace zaf {

/**
 Defines return values of a dialog box. 
 */
enum class DialogResult {

    /**
     The dialog box returns nothing.
     */
    None,

    /**
     The dialog box returns "OK".
     */
    Ok,

    /**
     The dialog box returns "Cancel".
     */
    Cancel,

    /**
     The dialog box returns "Yes".
     */
    Yes,

    /**
     The dialog box returns "No".
     */
    No,
};

}