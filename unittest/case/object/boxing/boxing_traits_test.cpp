#include <gtest/gtest.h>
#include <zaf/object/boxing/boxing_traits.h>
#include <zaf/graphic/point.h>

using namespace zaf;

//For reflective type.
static_assert(std::is_same_v<BoxingTraits<Point>::BoxedType, Point>);
static_assert(std::is_same_v<BoxingTraits<Point>::UnboxedType, Point>);
static_assert(std::is_same_v<BoxingTraits<Point>::BoxedInstanceType, std::shared_ptr<Point>>);

static_assert(std::is_same_v<BoxingTraits<const Point>::BoxedType, Point>);
static_assert(std::is_same_v<BoxingTraits<const Point>::UnboxedType, Point>);
static_assert(std::is_same_v<
    BoxingTraits<const Point>::BoxedInstanceType, 
    std::shared_ptr<Point>>);

static_assert(std::is_same_v<BoxingTraits<Point&>::BoxedType, Point>);
static_assert(std::is_same_v<BoxingTraits<Point&>::UnboxedType, Point>);
static_assert(std::is_same_v<BoxingTraits<Point&>::BoxedInstanceType, std::shared_ptr<Point>>);

//For custom boxed type.
static_assert(std::is_same_v<BoxingTraits<bool>::BoxedType, Boolean>);
static_assert(std::is_same_v<BoxingTraits<bool>::UnboxedType, bool>);
static_assert(std::is_same_v<BoxingTraits<bool>::BoxedInstanceType, std::shared_ptr<Boolean>>);

static_assert(std::is_same_v<BoxingTraits<const bool>::BoxedType, Boolean>);
static_assert(std::is_same_v<BoxingTraits<const bool>::UnboxedType, bool>);
static_assert(std::is_same_v<
    BoxingTraits<const bool>::BoxedInstanceType,
    std::shared_ptr<Boolean>>);

static_assert(std::is_same_v<BoxingTraits<bool&>::BoxedType, Boolean>);
static_assert(std::is_same_v<BoxingTraits<bool&>::UnboxedType, bool>);
static_assert(std::is_same_v<BoxingTraits<bool&>::BoxedInstanceType, std::shared_ptr<Boolean>>);


TEST(BoxingTraitsTest, ReflectiveTypeBoxing) {

    std::shared_ptr<Point> boxed_point = BoxingTraits<Point>::Box(Point{ 11, 22 });
    ASSERT_NE(boxed_point, nullptr);
    ASSERT_EQ(boxed_point->x, 11);
    ASSERT_EQ(boxed_point->y, 22);

    const Point* unboxed_point = BoxingTraits<Point>::Unbox(*boxed_point);
    ASSERT_EQ(unboxed_point, boxed_point.get());
    ASSERT_EQ(unboxed_point->x, 11);
    ASSERT_EQ(unboxed_point->y, 22);
}


TEST(BoxingTraitsTest, CustomBoxing) {

    std::shared_ptr<Boolean> boxed_bool = BoxingTraits<bool>::Box(true);
    ASSERT_NE(boxed_bool, nullptr);
    ASSERT_EQ(boxed_bool->Value(), true);

    const bool* unboxed_bool = BoxingTraits<bool>::Unbox(*boxed_bool);
    ASSERT_EQ(unboxed_bool, &boxed_bool->Value());
    ASSERT_EQ(*unboxed_bool, true);
}