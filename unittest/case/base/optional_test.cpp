#include <gtest/gtest.h>
#include <memory>
#include <zaf/base/optional.h>

class TestObject {
public:
    TestObject() {

    }

    TestObject(const TestObject& other) : value(other.value) {
        copy_constructor_called = true;
    }

    TestObject(TestObject&& other) : value(std::move(other.value)) {
        move_constructor_called = true;
    }

public:
    bool copy_constructor_called = false;
    bool move_constructor_called = false;
    std::string value;
};


TEST(Optional, DefaultConstruction) {

    static bool is_constructed = false;

    struct TestObject {
        TestObject() { is_constructed = true; }
    };

    zaf::optional<TestObject> optional;
    ASSERT_FALSE(optional.has_value());
    ASSERT_FALSE(is_constructed);
    ASSERT_THROW(optional.value(), zaf::bad_optional_access);
}


TEST(Optional, ConstructByCopyingFromObject) {

    const char* test_value = "Copy from object";
    TestObject test_object;
    test_object.value = test_value;

    zaf::optional<TestObject> optional(test_object);
    ASSERT_TRUE(optional.has_value());
    ASSERT_TRUE(optional->copy_constructor_called);
    ASSERT_EQ(optional->value, test_value);

    ASSERT_EQ(test_object.value, test_value);
}


TEST(Optional, ConstructByMovingFromObject) {

    const char* test_value = "Move from object";
    TestObject test_object;
    test_object.value = test_value;

    zaf::optional<TestObject> optional(std::move(test_object));
    ASSERT_TRUE(optional.has_value());
    ASSERT_TRUE(optional->move_constructor_called);
    ASSERT_EQ(optional->value, test_value);

    ASSERT_TRUE(test_object.value.empty());
}


TEST(Optional, AssignByCopyingFromObject) {

    const char* test_value = "Assign by copying from object";
    TestObject test_object;
    test_object.value = test_value;

    zaf::optional<TestObject> optional;
    optional = test_object;
    ASSERT_TRUE(optional.has_value());
    ASSERT_TRUE(optional->copy_constructor_called);
    ASSERT_EQ(optional->value, test_value);

    ASSERT_EQ(test_object.value, test_value);
}


TEST(Optional, AssignByMovingFromObject) {

    const char* test_value = "Assign by moving from object";
    TestObject test_object;
    test_object.value = test_value;

    zaf::optional<TestObject> optional;
    optional = std::move(test_object);
    ASSERT_TRUE(optional.has_value());
    ASSERT_TRUE(optional->move_constructor_called);
    ASSERT_EQ(optional->value, test_value);

    ASSERT_TRUE(test_object.value.empty());
}


TEST(Optional, Destruction) {

    static bool is_destructed = false;

    struct TestObject {
        ~TestObject() { is_destructed = true; }
    };

    TestObject test_object;
    auto optional = std::make_unique<zaf::optional<TestObject>>(test_object);
    ASSERT_FALSE(is_destructed);
    optional.reset();
    ASSERT_TRUE(is_destructed);
}