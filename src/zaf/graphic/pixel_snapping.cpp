#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/dpi.h>

namespace zaf {

Point SnapToPixels(const Point& point, float stroke_width, float dpi) noexcept {
    auto snapped_point_px = SnapAndTransformToPixels(point, stroke_width, dpi);
    return ToDIPs(snapped_point_px, dpi);
}


Point SnapAndTransformToPixels(const Point& point, float stroke_width, float dpi) noexcept {

    auto point_in_pixels = FromDIPs(point, dpi);
    auto offset_in_pixels = SnappingPixelOffsetForLine(stroke_width, dpi);

    Point result;
    result.x = std::round(point_in_pixels.x) + offset_in_pixels;
    result.y = std::round(point_in_pixels.y) + offset_in_pixels;
    return result;
}


Rect SnapToPixels(const Rect& rect, float stroke_width, float dpi) noexcept {
    auto snapped_rect_px = SnapAndTransformToPixels(rect, stroke_width, dpi);
    return ToDIPs(snapped_rect_px, dpi);
}


Rect SnapAndTransformToPixels(const Rect& rect, float stroke_width, float dpi) noexcept {

    auto rect_in_pixels = FromDIPs(rect, dpi);
    auto offset_in_pixels = SnappingPixelOffsetForLine(stroke_width, dpi);

    float left = std::round(rect_in_pixels.position.x) + offset_in_pixels;
    float top = std::round(rect_in_pixels.position.y) + offset_in_pixels;
    float right = std::round(rect_in_pixels.Right()) - offset_in_pixels;
    float bottom = std::round(rect_in_pixels.Bottom()) - offset_in_pixels;

    return Rect{ 
        left, 
        top,
        (std::max)(right - left, 0.f),
        (std::max)(bottom - top, 0.f)
    };
}


RoundedRect SnapToPixels(const RoundedRect& rounded_rect, float stroke_width, float dpi) noexcept {

    RoundedRect result;
    result.rect = SnapToPixels(rounded_rect.rect, stroke_width, dpi);

    auto x_radius_in_pixels = FromDIPs(rounded_rect.x_radius, dpi);
    auto y_radius_in_pixels = FromDIPs(rounded_rect.y_radius, dpi);

    auto aligned_x_radius = std::round(x_radius_in_pixels);
    auto aligned_y_radius = std::round(y_radius_in_pixels);

    result.x_radius = ToDIPs(aligned_x_radius, dpi);
    result.y_radius = ToDIPs(aligned_y_radius, dpi);
    return result;
}


Ellipse SnapToPixels(const Ellipse& ellipse, float stroke_width, float dpi) noexcept {

    auto position_in_pixels = FromDIPs(ellipse.position, dpi);
    auto x_radius_in_pixels = FromDIPs(ellipse.x_radius, dpi);
    auto y_radius_in_pixels = FromDIPs(ellipse.y_radius, dpi);

    auto offset_in_pixels = SnappingPixelOffsetForLine(stroke_width, dpi);

    float left = std::round(position_in_pixels.x - x_radius_in_pixels) + offset_in_pixels;
    float top = std::round(position_in_pixels.y - y_radius_in_pixels) + offset_in_pixels;
    float right = std::round(position_in_pixels.x + x_radius_in_pixels) - offset_in_pixels;
    float bottom = std::round(position_in_pixels.y + y_radius_in_pixels) - offset_in_pixels;

    float new_x_radius = (right - left) / 2;
    float new_y_radius = (bottom - top) / 2;
    Point new_position;
    new_position.x = left + new_x_radius;
    new_position.y = top + new_y_radius;

    Ellipse result;
    result.position = ToDIPs(new_position, dpi);
    result.x_radius = ToDIPs(new_x_radius, dpi);
    result.y_radius = ToDIPs(new_y_radius, dpi);
    return result;
}


float SnappingPixelOffsetForLine(float stroke_width, float dpi) noexcept {

    if (static_cast<int>(std::ceil(stroke_width)) % 2 != 0) {
        return FromDIPs(0.5f, dpi);
    }
    else {
        return 0;
    }
}

}