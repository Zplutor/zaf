#include <gtest/gtest.h>
#include <zaf/object/property_value_traits.h>
#include <zaf/graphic/size.h>

using namespace zaf;

//Reflective type.
static_assert(std::is_same_v<PropertyValueTraits<Size>::ValueType, Size>);
static_assert(std::is_same_v<PropertyValueTraits<Size&>::ValueType, Size>);
static_assert(std::is_same_v<PropertyValueTraits<const Size>::ValueType, Size>);

static_assert(std::is_same_v<PropertyValueTraits<Size>::BoxedType, Size>);
static_assert(std::is_same_v<PropertyValueTraits<Size&>::BoxedType, Size>);
static_assert(std::is_same_v<PropertyValueTraits<const Size>::BoxedType, Size>);

//Custom boxing type.
static_assert(std::is_same_v<PropertyValueTraits<bool>::ValueType, bool>);
static_assert(std::is_same_v<PropertyValueTraits<bool&>::ValueType, bool>);
static_assert(std::is_same_v<PropertyValueTraits<const bool>::ValueType, bool>);

static_assert(std::is_same_v<PropertyValueTraits<bool>::BoxedType, Boolean>);
static_assert(std::is_same_v<PropertyValueTraits<bool&>::BoxedType, Boolean>);
static_assert(std::is_same_v<PropertyValueTraits<const bool>::BoxedType, Boolean>);

//Shared pointer to reflective type.
static_assert(std::is_same_v<
    PropertyValueTraits<std::shared_ptr<Size>>::ValueType, 
    std::shared_ptr<Size>>);

static_assert(std::is_same_v<
    PropertyValueTraits<std::shared_ptr<Size>&>::ValueType,
    std::shared_ptr<Size>>);

static_assert(std::is_same_v<
    PropertyValueTraits<const std::shared_ptr<Size>>::ValueType,
    std::shared_ptr<Size>>);

static_assert(std::is_same_v<
    PropertyValueTraits<std::shared_ptr<const Size>>::ValueType,
    std::shared_ptr<const Size>>);

static_assert(std::is_same_v<PropertyValueTraits<std::shared_ptr<Size>>::BoxedType, Size>);
static_assert(std::is_same_v<PropertyValueTraits<std::shared_ptr<Size>&>::BoxedType, Size>);
static_assert(std::is_same_v<PropertyValueTraits<const std::shared_ptr<Size>>::BoxedType, Size>);
static_assert(std::is_same_v<
    PropertyValueTraits<std::shared_ptr<const Size>>::BoxedType,
    const Size>);

//Custom property value.
namespace {

struct CustomPropertyValue { 
    int value{};
};

struct CustomPropertyObject : zaf::Object {
    int value{};
};

template<>
struct zaf__CustomPropertyValueTraits<CustomPropertyValue> {

    using BoxedType = CustomPropertyObject;

    static std::shared_ptr<CustomPropertyObject> ToBoxedObject(CustomPropertyValue value) {
        auto result = std::make_shared<CustomPropertyObject>();
        result->value = value.value;
        return result;
    }

    static CustomPropertyValue FromBoxedObject(
        const std::shared_ptr<CustomPropertyObject>& object) {
        return CustomPropertyValue{ object->value };
    }
};

}


static_assert(std::is_same_v<
    PropertyValueTraits<CustomPropertyValue>::ValueType, 
    CustomPropertyValue>);

static_assert(std::is_same_v<
    PropertyValueTraits<CustomPropertyValue&>::ValueType,
    CustomPropertyValue>);

static_assert(std::is_same_v<
    PropertyValueTraits<const CustomPropertyValue>::ValueType,
    CustomPropertyValue>);

static_assert(std::is_same_v<
    PropertyValueTraits<CustomPropertyValue>::BoxedType,
    CustomPropertyObject>);

static_assert(std::is_same_v<
    PropertyValueTraits<CustomPropertyValue&>::BoxedType,
    CustomPropertyObject>);

static_assert(std::is_same_v<
    PropertyValueTraits<const CustomPropertyValue>::BoxedType,
    CustomPropertyObject>);


TEST(PropertyValueTraitsTest, ReflectiveTypeBoxing) {

    using Traits = PropertyValueTraits<Size>;

    std::shared_ptr<Object> boxed_object = Traits::ToBoxedObject(Size{ 10, 20 });
    std::shared_ptr<Size> boxed_size = As<Size>(boxed_object);
    ASSERT_NE(boxed_size, nullptr);
    ASSERT_EQ(boxed_size->width, 10);
    ASSERT_EQ(boxed_size->height, 20);

    auto unboxed_size = Traits::FromBoxedObject(boxed_object);
    ASSERT_EQ(unboxed_size.width, 10);
    ASSERT_EQ(unboxed_size.height, 20);
}


TEST(PropertyValueTraitsTest, CustomBoxingTypeBoxing) {

    using Traits = PropertyValueTraits<bool>;

    std::shared_ptr<Object> boxed_object = Traits::ToBoxedObject(true);
    std::shared_ptr<Boolean> boxed_bool = As<Boolean>(boxed_object);
    ASSERT_NE(boxed_bool, nullptr);
    ASSERT_EQ(boxed_bool->Value(), true);

    auto unboxed_bool = Traits::FromBoxedObject(boxed_object);
    ASSERT_EQ(unboxed_bool, true);
}


TEST(PropertyValueTraitsTest, BoxedInstanceBoxing) {

    using Traits = PropertyValueTraits<std::shared_ptr<Size>>;

    auto size = std::make_shared<Size>(30.f, 40.f);
    std::shared_ptr<Object> boxed_object = Traits::ToBoxedObject(std::shared_ptr<Size>(size));
    std::shared_ptr<Size> boxed_size = As<Size>(boxed_object);
    ASSERT_EQ(boxed_size, size);

    auto unboxed_size = Traits::FromBoxedObject(boxed_object);
    ASSERT_EQ(unboxed_size, size);
}


TEST(PropertyValueTraitsTest, CustomPropertyValueBoxing) {

    using Traits = PropertyValueTraits<CustomPropertyValue>;

    CustomPropertyValue value{ 45 };
    auto boxed_object = Traits::ToBoxedObject(CustomPropertyValue(value));
    auto custom_boxed_object = As<CustomPropertyValue>(boxed_object);
    ASSERT_NE(custom_boxed_object, nullptr);
    ASSERT_EQ(custom_boxed_object->value, 45);

    auto unboxed_value = Traits::FromBoxedObject(boxed_object);
    ASSERT_EQ(unboxed_value.value, 45);
}