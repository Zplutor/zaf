#include <gtest/gtest.h>
#include <zaf/graphic/dwrite/inline_object_metrics.h>

using namespace zaf::dwrite;

TEST(InlineObjectMetricsTest, DefaultConstruct) {

    InlineObjectMetrics metrics;
    ASSERT_EQ(metrics.Width(), 0);
    ASSERT_EQ(metrics.Height(), 0);
    ASSERT_EQ(metrics.Baseline(), 0);
    ASSERT_EQ(metrics.Inner().supportsSideways, FALSE);
}