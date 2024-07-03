#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/enum_support.h>
#include <zaf/object/parsing/xaml_reader.h>

namespace {

enum class TestType {
    First,
    Second,
    Third,

    One = First,
    Two = Second,
    Three = Third,
};

ZAF_ENUM_BEGIN(TestType)
ZAF_ENUM_CONSTANT(First)
ZAF_ENUM_CONSTANT(Second)
ZAF_ENUM_CONSTANT(Third)
ZAF_ENUM_CONSTANT(One)
ZAF_ENUM_CONSTANT(Two)
ZAF_ENUM_CONSTANT(Three)
ZAF_ENUM_END;

ZAF_ENUM_IMPL(TestType);

}

namespace ns {

enum class NamespaceTestType {
    First,
    Second,
    Third,
};

ZAF_ENUM_BEGIN(NamespaceTestType)
ZAF_ENUM_CONSTANT(First)
ZAF_ENUM_CONSTANT(Second)
ZAF_ENUM_CONSTANT(Third)
ZAF_ENUM_END;

ZAF_ENUM_IMPL(NamespaceTestType);

}


TEST(EnumTest, Boxing) {

    auto boxed = zaf::Box(TestType::First);
    auto enum_object = dynamic_cast<TestTypeEnum*>(boxed.get());
    ASSERT_NE(enum_object, nullptr);

    auto namespace_boxed = zaf::Box(ns::NamespaceTestType::Second);
    auto namespace_enum_object = dynamic_cast<ns::NamespaceTestTypeEnum*>(namespace_boxed.get());
    ASSERT_NE(namespace_enum_object, nullptr);
}


TEST(EnumTest, ToString) {

    ASSERT_EQ(zaf::Box(TestType::First)->ToString(), L"First");
    ASSERT_EQ(zaf::Box(TestType::Second)->ToString(), L"Second");
    ASSERT_EQ(zaf::Box(TestType::Third)->ToString(), L"Third");

    //Alias enum value still generates original name.
    ASSERT_EQ(zaf::Box(TestType::One)->ToString(), L"First");
    ASSERT_EQ(zaf::Box(TestType::Two)->ToString(), L"Second");
    ASSERT_EQ(zaf::Box(TestType::Three)->ToString(), L"Third");

    ASSERT_EQ(zaf::Box(static_cast<TestType>(1374))->ToString(), L"1374");
}


TEST(EnumTest, GetValue) {

    auto object = TestTypeEnum::EnumType()->GetValue(L"First");
    ASSERT_NE(object, nullptr);
    auto value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::First);

    object = TestTypeEnum::EnumType()->GetValue(L"Second");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Second);

    object = TestTypeEnum::EnumType()->GetValue(L"Third");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Third);

    object = TestTypeEnum::EnumType()->GetValue(L"Fourth");
    ASSERT_EQ(object, nullptr);

    //Alias
    object = TestTypeEnum::EnumType()->GetValue(L"One");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::One);

    object = TestTypeEnum::EnumType()->GetValue(L"Two");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Two);

    object = TestTypeEnum::EnumType()->GetValue(L"Three");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Three);
}


TEST(EnumTest, Values) {

    auto all_values = TestTypeEnum::EnumType()->Values();
    ASSERT_EQ(all_values.size(), 3);
    ASSERT_EQ(zaf::Unbox<TestType>(*all_values[0]), TestType::First);
    ASSERT_EQ(zaf::Unbox<TestType>(*all_values[1]), TestType::Second);
    ASSERT_EQ(zaf::Unbox<TestType>(*all_values[2]), TestType::Third);
}


TEST(EnumTest, Parse) {

    auto parser = TestTypeEnum::EnumType()->Parser();

    TestTypeEnum enum_object;

    parser->ParseFromAttribute(L"First", enum_object);
    ASSERT_EQ(enum_object.Value(), TestType::First);

    parser->ParseFromAttribute(L"Second", enum_object);
    ASSERT_EQ(enum_object.Value(), TestType::Second);

    parser->ParseFromAttribute(L"Third", enum_object);
    ASSERT_EQ(enum_object.Value(), TestType::Third);

    ASSERT_THROW(parser->ParseFromAttribute(L"Fourth", enum_object), zaf::ParseError);
}


TEST(EnumTest, ParseToInvalidObject) {

    auto parser = TestTypeEnum::EnumType()->Parser();
    zaf::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"First", object), zaf::InvalidTypeError);

    auto node = zaf::XamlReader::FromString("<TestType>Second</TestType>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, object), zaf::InvalidTypeError);
}