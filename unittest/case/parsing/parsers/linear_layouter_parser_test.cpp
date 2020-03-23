#include <gtest/gtest.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/parsing/parsers/linear_layouter_parser.h>
#include "utility.h"

using namespace zaf;

TEST(LinearLayouterPraserTest, ParseDirection) {

    auto result = TestEnumProperty<LinearLayouter, LayoutDirection>(
        "Direction", 
        {
            { LayoutDirection::LeftToRight, "LeftToRight" },
            { LayoutDirection::TopToBottom, "TopToBottom" },
        },
        [](LinearLayouter& layouter) {
            return layouter.GetDirection();
        }
    );
    ASSERT_TRUE(result);
}


TEST(LinearLayouterPraserTest, ParseControlAlignment) {

    auto result = TestEnumProperty<LinearLayouter, ControlAlignment>(
        "ControlAlignment",
        {
            { ControlAlignment::Leading, "Leading" },
            { ControlAlignment::Tailing, "Tailing" },
            { ControlAlignment::Center, "Center" },
        },
        [](LinearLayouter& layouter) {
            return layouter.GetControlAlignment();
        }
    );
    ASSERT_TRUE(result);
}


TEST(LinearLayouterPraserTest, ParseAxisAlignment) {

    auto result = TestEnumProperty<LinearLayouter, AxisAlignment>(
        "AxisAlignment",
        {
            { AxisAlignment::Near, "Near" },
            { AxisAlignment::Far, "Far" },
            { AxisAlignment::Center, "Center" },
        },
        [](LinearLayouter& layouter) {
            return layouter.GetAxisAlignment();
        }
    );
    ASSERT_TRUE(result);
}