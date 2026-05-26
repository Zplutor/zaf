#pragma once

/**
@file
    Defines the `zaf::PixelSnapMode` enum class.
*/

namespace zaf {

/**
Specifies the pixel snap mode of a canvas.

@see zaf::Canvas
@see zaf::CanvasState
*/
enum class PixelSnapMode {

    /**
    The canvas doesn't snap the painting to pixels.
    */
    NoSnap,

    /**
    The canvas tries to snap the painting to pixels.
    */
    Snap,
};

}