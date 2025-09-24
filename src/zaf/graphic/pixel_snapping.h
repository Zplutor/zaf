#pragma once

/**
@file
    Defines functions related to pixel snapping.
*/

#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

/**
Snaps a point to pixels for drawing lines.

@param point
    The point in DIPs to snap.

@param stroke_width
    The stroke width, in DIPs, of the line that the point belongs to.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped point in DIPs.
*/
Point SnapToPixels(const Point& point, float stroke_width, float dpi) noexcept;


/**
Snaps a point to pixels for drawing shapes other than lines.

@param point
    The point in DIPs to snap.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped point in DIPs.
*/
inline Point SnapToPixels(const Point& point, float dpi) noexcept {
    return SnapToPixels(point, 0, dpi);
}


/**
Snaps a point to pixels for drawing lines, and transforms the result to pixel values.

@param point
    The point in DIPs to snap.

@param stroke_width
    The stroke width, in DIPs, of the line that the point belongs to.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped point in pixels.
*/
Point SnapAndTransformToPixels(const Point& point, float stroke_width, float dpi) noexcept;


/**
Snaps a point to pixels for drawing shapes other than lines, and transforms the result to pixel 
values.

@param point
    The point in DIPs to snap.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped point in pixels.
*/
inline Point SnapAndTransformToPixels(const Point& point, float dpi) noexcept {
    return SnapAndTransformToPixels(point, 0, dpi);
}


/**
Snaps a rectangle to pixels for drawing its frame.

@param rect
    The rectangle in DIPs to snap.

@param stroke_width
    The stroke width, in DIPs, of the frame.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped rectangle in DIPs.
*/
Rect SnapToPixels(const Rect& rect, float stroke_width, float dpi) noexcept;


/**
Snaps a rectangle to pixels for drawing it, excluding its frame.

@param rect
    The rectangle in DIPs to snap.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped rectangle in DIPs.
*/
inline Rect SnapToPixels(const Rect& rect, float dpi) noexcept {
    return SnapToPixels(rect, 0, dpi);
}


/**
Snaps a rectangle to pixels for drawing its frame, and transforms the result to pixel values.

@param rect
    The rectangle in DIPs to snap.

@param stroke_width
    The stroke width, in DIPs, of the frame.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped rectangle in pixels.
*/
Rect SnapAndTransformToPixels(const Rect& rect, float stroke_width, float dpi) noexcept;


/**
Snaps a rectangle to pixels for drawing it, excluding its frame, and transforms the result to pixel 
values.

@param rect
    The rectangle in DIPs to snap.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped rectangle in pixels.
*/
inline Rect SnapAndTransformToPixels(const Rect& rect, float dpi) noexcept {
    return SnapAndTransformToPixels(rect, 0, dpi);
}


/**
Snaps a rounded rectangle to pixels for drawing its frame.

@param rounded_rect
    The rounded rectangle in DIPs to snap.

@param stroke_width
    The stroke width, in DIPs, of the frame.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped rounded rectangle in DIPs.
*/
RoundedRect SnapToPixels(const RoundedRect& rounded_rect, float stroke_width, float dpi) noexcept;


/**
Snaps a rounded rectangle to pixels for drawing it, excluding its frame.

@param rounded_rect
    The rounded rectangle in DIPs to snap.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped rounded rectangle in DIPs.
*/
inline RoundedRect SnapToPixels(const RoundedRect& rounded_rect, float dpi) noexcept {
    return SnapToPixels(rounded_rect, 0, dpi);
}


/**
Snaps an ellipse to pixels for drawing its frame.

@param ellipse
    The ellipse in DIPs to snap.

@param stroke_width
    The stroke width, in DIPs, of the frame.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped ellipse in DIPs.
*/
Ellipse SnapToPixels(const Ellipse& ellipse, float stroke_width, float dpi) noexcept;


/**
Snaps an ellipse to pixels for drawing it, excluding its frame.

@param ellipse
    The ellipse in DIPs to snap.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapped ellipse in DIPs.
*/
inline Ellipse SnapToPixels(const Ellipse& ellipse, float dpi) noexcept {
    return SnapToPixels(ellipse, 0, dpi);
}


/**
Calculates the snapping offset for drawing a line.

@param stroke_width
    The stroke width, in DIPs, of the line.

@param dpi
    The DPI value used to transform between the DIP values and pixel values.

@return
    The snapping offset in pixels.
*/
float SnappingPixelOffsetForLine(float stroke_width, float dpi) noexcept;

}