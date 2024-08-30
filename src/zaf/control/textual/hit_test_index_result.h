#pragma once

#include <zaf/graphic/rect.h>

namespace zaf::textual {

/**
Contains the result of a hit test by index on the text box.
*/
class HitTestIndexResult {
public:
    /**
    Constructs the instance with the result.
    */
    HitTestIndexResult(const zaf::Rect& rect, bool is_text) : rect_(rect), is_text_(is_text) { }

    /**
    The rectangle area of the character specified by the index.
    */
    const zaf::Rect& Rect() const {
        return rect_;
    }

    /**
    Indicates whether the index is on a text character.
    */
    bool IsText() const {
        return is_text_;
    }

private:
    zaf::Rect rect_;
    bool is_text_{};
};

}