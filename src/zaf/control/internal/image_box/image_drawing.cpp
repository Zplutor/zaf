#include <zaf/control/internal/image_box/image_drawing.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {
namespace {

Rect MakeCenteredImageRect(const Rect& draw_rect, const Size& image_size) {

    Rect image_rect;
    image_rect.position.x = draw_rect.position.x + (draw_rect.size.width - image_size.width) / 2;
    image_rect.position.y = draw_rect.position.y + (draw_rect.size.height - image_size.height) / 2;
    image_rect.size = image_size;
    return image_rect;
}


void DrawImageWithCenterLayout(
    Canvas& canvas,
    const Rect& draw_rect,
    const d2d::RenderBitmap& bitmap,
    d2d::InterpolationMode interpolation_mode) {

    auto image_rect = MakeCenteredImageRect(draw_rect, bitmap.GetSize());
    canvas.DrawBitmap(bitmap, image_rect, DrawImageOptions().InterpolationMode(interpolation_mode));
}


void DrawImageWithZoomLayout(
    Canvas& canvas,
    const Rect& draw_rect,
    const d2d::RenderBitmap& bitmap,
    d2d::InterpolationMode interpolation_mode) {

    auto image_size = bitmap.GetSize();

    float width_percent = image_size.width / draw_rect.size.width;
    float height_percent = image_size.height / draw_rect.size.height;

    float zoom_percent = (std::max)(width_percent, height_percent);
    float zoomed_width = image_size.width / zoom_percent;
    float zoomed_height = image_size.height / zoom_percent;
    
    auto image_rect = MakeCenteredImageRect(draw_rect, Size{ zoomed_width, zoomed_height });
    canvas.DrawBitmap(bitmap, image_rect, DrawImageOptions().InterpolationMode(interpolation_mode));
}


void DrawImageWithTileLayout(
    Canvas& canvas,
    const Rect& draw_rect,
    const d2d::RenderBitmap& bitmap,
    d2d::InterpolationMode interpolation_mode) {

    auto image_size = bitmap.GetSize();
    auto positions = CalculateTiledImagePositions(draw_rect, image_size);

    Rect image_rect;
    image_rect.size = image_size;

    DrawImageOptions options;
    options.InterpolationMode(interpolation_mode);

    for (const auto& each_position : positions) {
        image_rect.position = each_position;
        canvas.DrawBitmap(bitmap, image_rect, options);
    }
}

}

void DrawImage(
    Canvas& canvas,
    const Rect& draw_rect,
    ImageLayout image_layout,
    const d2d::RenderBitmap& bitmap,
    d2d::InterpolationMode interpolation_mode) {

    if (image_layout == ImageLayout::None) {

        Rect image_rect{ draw_rect.position, bitmap.GetSize() };
        canvas.DrawBitmap(
            bitmap, 
            image_rect,
            DrawImageOptions().InterpolationMode(interpolation_mode));
    }
    else if (image_layout == ImageLayout::Stretch) {

        canvas.DrawBitmap(
            bitmap,
            draw_rect, 
            DrawImageOptions().InterpolationMode(interpolation_mode));
    }
    else if (image_layout == ImageLayout::Center) {
        DrawImageWithCenterLayout(canvas, draw_rect, bitmap, interpolation_mode);
    }
    else if (image_layout == ImageLayout::Zoom) {
        DrawImageWithZoomLayout(canvas, draw_rect, bitmap, interpolation_mode);
    }
    else if (image_layout == ImageLayout::Tile) {
        DrawImageWithTileLayout(canvas, draw_rect, bitmap, interpolation_mode);
    }
    else {
        ZAF_ALERT();
    }
}


std::vector<Point> CalculateTiledImagePositions(const Rect& draw_rect, const Size& image_size) {

    float width_integral{};
    float width_fractional = 
        std::modf(draw_rect.size.width / image_size.width, &width_integral);

    float height_integral{};
    float height_fractional = 
        std::modf(draw_rect.size.height / image_size.height, &height_integral);

    auto column_count = static_cast<int>(width_integral);
    if (width_fractional > 0.f) {
        column_count++;
    }

    auto row_count = static_cast<int>(height_integral);
    if (height_fractional > 0.f) {
        row_count++;
    }

    std::vector<Point> result;
    for (int row_index = 0; row_index < row_count; ++row_index) {

        float y = draw_rect.position.y + image_size.height * row_index;

        for (int column_index = 0; column_index < column_count; ++column_index) {

            float x = draw_rect.position.x + image_size.width * column_index;
            result.emplace_back(x, y);
        }
    }
    return result;
}

}