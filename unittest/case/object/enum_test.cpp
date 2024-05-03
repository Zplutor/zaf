#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_definition.h>
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

ZAF_DECLARE_ENUM(TestType)

ZAF_DEFINE_ENUM(TestType)
ZAF_DEFINE_ENUM_VALUE(First)
ZAF_DEFINE_ENUM_VALUE(Second)
ZAF_DEFINE_ENUM_VALUE(Third)
ZAF_DEFINE_ENUM_VALUE(One)
ZAF_DEFINE_ENUM_VALUE(Two)
ZAF_DEFINE_ENUM_VALUE(Three)
ZAF_DEFINE_ENUM_END

}

namespace ns {

enum class NamespaceTestType {
    First,
    Second,
    Third,
};

ZAF_DECLARE_ENUM(NamespaceTestType)

ZAF_DEFINE_ENUM(NamespaceTestType)
ZAF_DEFINE_ENUM_VALUE(First)
ZAF_DEFINE_ENUM_VALUE(Second)
ZAF_DEFINE_ENUM_VALUE(Third)
ZAF_DEFINE_ENUM_END

}

ZAF_ENABLE_ENUM_BOXING(TestType)
ZAF_ENABLE_ENUM_BOXING_WITH_NAMESPACE(ns, NamespaceTestType)


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


TEST(EnumTest, FindValue) {

    auto object = TestTypeEnum::EnumType()->FindValue(L"First");
    ASSERT_NE(object, nullptr);
    auto value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::First);

    object = TestTypeEnum::EnumType()->FindValue(L"Second");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Second);

    object = TestTypeEnum::EnumType()->FindValue(L"Third");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Third);

    object = TestTypeEnum::EnumType()->FindValue(L"Fourth");
    ASSERT_EQ(object, nullptr);

    //Alias
    object = TestTypeEnum::EnumType()->FindValue(L"One");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::One);

    object = TestTypeEnum::EnumType()->FindValue(L"Two");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Two);

    object = TestTypeEnum::EnumType()->FindValue(L"Three");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(*value, TestType::Three);
}


TEST(EnumTest, GetAllValues) {

    auto all_values = TestTypeEnum::EnumType()->GetAllValues();
    ASSERT_EQ(all_values.size(), 3);
    ASSERT_EQ(zaf::Unbox<TestType>(*all_values[0]), TestType::First);
    ASSERT_EQ(zaf::Unbox<TestType>(*all_values[1]), TestType::Second);
    ASSERT_EQ(zaf::Unbox<TestType>(*all_values[2]), TestType::Third);
}


TEST(EnumTest, Parse) {

    auto parser = TestTypeEnum::EnumType()->GetParser();

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

    auto parser = TestTypeEnum::EnumType()->GetParser();
    zaf::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"First", object), zaf::InvalidTypeError);

    auto node = zaf::XamlReader::FromString("<TestType>Second</TestType>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, object), zaf::InvalidTypeError);
}