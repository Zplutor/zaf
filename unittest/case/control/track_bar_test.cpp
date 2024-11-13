#include <gtest/gtest.h>
#include <zaf/control/track_bar.h>

using namespace zaf;

TEST(TrackBarTest, Value) {

    auto track_bar = Create<TrackBar>();
    ASSERT_EQ(track_bar->Value(), 0);
}


TEST(TrackBarTest, MinValue) {

    auto track_bar = Create<TrackBar>();
    ASSERT_EQ(track_bar->MinValue(), 0);
}


TEST(TrackBarTest, MaxValue) {

    auto track_bar = Create<TrackBar>();
    ASSERT_EQ(track_bar->MaxValue(), 10);
}


TEST(TrackBarTest, Focus) {

    auto track_bar = Create<TrackBar>();
    ASSERT_EQ(track_bar->CanFocus(), true);
}