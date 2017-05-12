#include <gtest/gtest.h>
#include <zaf/base/any.h>
#include "test_object.h"

static bool CheckTestObjectInAny(const zaf::any& any, bool check_copy_constructor_called, const std::string& value);

TEST(Any, DefaultConstruction) {

    zaf::any any;

    ASSERT_FALSE(any.has_value());
    ASSERT_EQ(any.type(), typeid(void));
}


TEST(Any, ConstructByCopyingOther) {

    TestObject test_object;
    test_object.value = "copy";

    zaf::any other(test_object);
    zaf::any any(other);

    ASSERT_TRUE(CheckTestObjectInAny(other, true, "copy"));
    ASSERT_TRUE(CheckTestObjectInAny(any, true, "copy"));
}


TEST(Any, ConstructByMovingOther) {

    TestObject test_object;
    test_object.value = "move";

    zaf::any other(test_object);
    zaf::any any(std::move(other));

    ASSERT_FALSE(other.has_value());
    //holders in two anys are exchanged, so their values are not changed.
    ASSERT_TRUE(CheckTestObjectInAny(any, true, "move")); 
}


TEST(Any, ConstructByCopyingValue) {

    TestObject test_object;
    test_object.value = "any";

    zaf::any any(test_object);

    ASSERT_TRUE(CheckTestObjectInAny(any, true, "any"));
}


TEST(Any, ConstructByMovingValue) {

    TestObject test_object;
    test_object.value = "any";

    zaf::any any(std::move(test_object));

    ASSERT_TRUE(test_object.value.empty());
    ASSERT_TRUE(CheckTestObjectInAny(any, false, "any"));
}


TEST(Any, AssignByCopyingFromOther) {

    TestObject test_object;
    test_object.value = "AssignByCopyingFromOther";

    zaf::any other(test_object);
    zaf::any any;
    any = other;

    ASSERT_TRUE(CheckTestObjectInAny(other, true, "AssignByCopyingFromOther"));
    ASSERT_TRUE(CheckTestObjectInAny(any, true, "AssignByCopyingFromOther"));
}


TEST(Any, AssignByMovingFromOther) {

    TestObject test_object;
    test_object.value = "AssignByMovingFromOther";

    zaf::any other(test_object);
    zaf::any any;
    any = std::move(other);

    ASSERT_FALSE(other.has_value());
    ASSERT_TRUE(CheckTestObjectInAny(any, true, "AssignByMovingFromOther"));
}


TEST(Any, AssignByCopyingFromValue) {

    TestObject test_object;
    test_object.value = "AssignByCopyingFromValue";

    zaf::any any;
    any = test_object;

    ASSERT_EQ(test_object.value, "AssignByCopyingFromValue");
    ASSERT_TRUE(CheckTestObjectInAny(any, true, "AssignByCopyingFromValue"));
}


TEST(Any, AssignByMovingFromValue) {

    TestObject test_object;
    test_object.value = "AssignByMovingFromValue";

    zaf::any any;
    any = std::move(test_object);

    ASSERT_TRUE(test_object.value.empty());
    ASSERT_TRUE(CheckTestObjectInAny(any, false, "AssignByMovingFromValue"));
}


TEST(Any, Swap) {

    TestObject test_object1;
    test_object1.value = "value1";
    zaf::any any1(test_object1);

    TestObject test_object2;
    test_object2.value = "value2";
    zaf::any any2(test_object2);

    any1.swap(any2);

    ASSERT_TRUE(CheckTestObjectInAny(any1, true, "value2"));
    ASSERT_TRUE(CheckTestObjectInAny(any2, true, "value1"));
}


TEST(Any, Reset) {

    zaf::any any(1);
    ASSERT_TRUE(any.has_value());
    any.reset();
    ASSERT_FALSE(any.has_value());
}


TEST(Any, AnyCastWithPointer) {

    TestObject test_object;
    test_object.value = "AnyCastWithPointer";
    zaf::any any(test_object);

    auto value = zaf::any_cast<TestObject>(&any);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(value->value, "AnyCastWithPointer");

    auto int_value = zaf::any_cast<int>(&any);
    ASSERT_EQ(int_value, nullptr);
}


TEST(Any, AnyCastWithConstPointer) {

    TestObject test_object;
    test_object.value = "AnyCastWithConstPointer";
    const zaf::any any(test_object);

    auto value = zaf::any_cast<TestObject>(&any);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(value->value, "AnyCastWithConstPointer");

    auto int_value = zaf::any_cast<int>(&any);
    ASSERT_EQ(int_value, nullptr);
}


TEST(Any, AnyCastWithReference) {

    TestObject test_object;
    test_object.value = "AnyCastWithReference";
    zaf::any any(test_object);

    auto value = zaf::any_cast<TestObject>(any);
    ASSERT_EQ(value.value, "AnyCastWithReference");
    ASSERT_TRUE(CheckTestObjectInAny(any, true, "AnyCastWithReference"));

    ASSERT_THROW(zaf::any_cast<int>(any), zaf::bad_any_cast);
}


TEST(Any, AnyCastWithRightValueReference) {

    TestObject test_object;
    test_object.value = "AnyCastWithRightValueReference";
    zaf::any any(test_object);

    auto value = zaf::any_cast<TestObject>(std::move(any));
    ASSERT_EQ(value.value, "AnyCastWithRightValueReference");
    ASSERT_TRUE(CheckTestObjectInAny(any, true, ""));

    ASSERT_THROW(zaf::any_cast<int>(std::move(any)), zaf::bad_any_cast);
}


static bool CheckTestObjectInAny(const zaf::any& any, bool check_copy_constructor_called, const std::string& value) {

    if (!any.has_value()) {
        return false;
    }

    if (any.type() != typeid(TestObject)) {
        return false;
    }

    auto test_object_pointer = zaf::any_cast<TestObject>(&any);
    if (test_object_pointer == nullptr) {
        return false;
    }

    if (check_copy_constructor_called) {
        if (!test_object_pointer->copy_constructor_called) {
            return false;
        }
    }
    else {
        if (!test_object_pointer->move_constructor_called) {
            return false;
        }
    }

    return test_object_pointer->value == value;
}