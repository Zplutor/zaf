#include <gtest/gtest.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/control/internal/image_box/image_drawing.h>

using namespace zaf;

TEST(ImageDrawingTest, CalculateTiledImagePositions) {

    Rect draw_rect{ 2, 3, 10, 10 };
    Size image_size{ 3, 3 };

    auto positions = internal::CalculateTiledImagePositions(draw_rect, image_size);

    std::vector<Point> expected_positions{
        Point{ 2, 3 },
        Point{ 5, 3 },
        Point{ 8, 3 },
        Point{ 11, 3 },
        Point{ 2, 6 },
        Point{ 5, 6 },
        Point{ 8, 6 },
        Point{ 11, 6 },
        Point{ 2, 9 },
        Point{ 5, 9 },
        Point{ 8, 9 },
        Point{ 11, 9 },
        Point{ 2, 12 },
        Point{ 5, 12 },
        Point{ 8, 12 },
        Point{ 11, 12 },
    };

    Sort(positions);
    Sort(expected_positions);
    ASSERT_EQ(positions, expected_positions);
}