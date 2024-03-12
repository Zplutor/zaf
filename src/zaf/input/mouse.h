#pragma once

#include <zaf/input/cursor.h>

namespace zaf {

class Mouse {
public:
    static void SetCursor(Cursor cursor) noexcept {
        ::SetCursor(cursor.Handle());
    }
    
public:
    Mouse() = delete;
};

}