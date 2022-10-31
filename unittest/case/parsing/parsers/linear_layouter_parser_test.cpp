#include <gtest/gtest.h>
#include <zaf/control/layout/linear_layouter.h>
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
            return layouter.Direction();
        }
    );
    ASSERT_TRUE(result);
}


TEST(LinearLayouterPraserTest, ParseAxisAlignment) {

    auto result = TestEnumProperty<LinearLayouter, AxisAlignment>(
        "AxisAlignment",
        {
            { AxisAlignment::Start, "Start" },
            { AxisAlignment::End, "End" },
            { AxisAlignment::Center, "Center" },
        },
        [](LinearLayouter& layouter) {
            return layouter.AxisAlignment();
        }
    );
    ASSERT_TRUE(result);
}


TEST(LinearLayouterPraserTest, ParseCrossAxisAlignment) {

    auto result = TestEnumProperty<LinearLayouter, AxisAlignment>(
        "CrossAxisAlignment",
        {
            { AxisAlignment::Start, "Start" },
            { AxisAlignment::End, "End" },
            { AxisAlignment::Center, "Center" },
        },
        [](LinearLayouter& layouter) {
            return layouter.CrossAxisAlignment();
        }
    );
    ASSERT_TRUE(result);
}