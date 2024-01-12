#pragma once

#include <zaf/base/flag_enum.h>

namespace zaf::text_box {

/**
SelectionOption is used in TextBox's methods to control the behavior when changing the selection
range. It's a flag enum that can be used with bit-wise operators to combine multiple values.
*/
enum class SelectionOption {

    /**
    The caret of the text box will be set to the end of the selection range.
    */
    SetCaretToEnd = 0,
    
    /**
    The text box will not scroll even if the selection range is outside the visible area.
    */
    NoScroll = 0,

    /**
    The default selection option, equals to SetCaretToEnd | NoScroll.
    */
    Default = SetCaretToEnd | NoScroll,

    /**
    The caret of the text box will be set to the beginning of the selection range.
    */
    SetCaretToBeign = 1,

    /**
    The text box will scroll to the position of the caret if it is outside the visible area.
    */
    ScrollToCaret = 1 << 1,
};

ZAF_ENABLE_FLAG_ENUM(SelectionOption);

}