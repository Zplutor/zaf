#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/dynamic/boxing/boxing.h>
#include <zaf/dynamic/enum_support.h>
#include <zaf/dynamic/parsing/xaml_reader.h>

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


enum class FlagsType {
    Flag1 = 0x1,
    Flag2 = 0x2,
    Flag3 = 0x4,
};

ZAF_ENABLE_FLAGS_ENUM(FlagsType);

ZAF_ENUM_BEGIN(FlagsType);
ZAF_ENUM_CONSTANT(Flag1);
ZAF_ENUM_CONSTANT(Flag2);
ZAF_ENUM_CONSTANT(Flag3);
ZAF_ENUM_END;

ZAF_ENUM_IMPL(FlagsType);

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


TEST(EnumTest, EnumType) {

    {
        auto type = TestTypeEnum::EnumType();
        ASSERT_EQ(type->Name(), L"TestType");
        ASSERT_FALSE(type->IsFlagsEnum());
    }

    {
        auto type = FlagsTypeEnum::EnumType();
        ASSERT_EQ(type->Name(), L"FlagsType");
        ASSERT_TRUE(type->IsFlagsEnum());
    }
}


TEST(EnumTest, EnumConstants) {

    auto type = TestTypeEnum::Type::Instance();

    ASSERT_EQ(type->FirstConstant->Name(), L"First");
    ASSERT_EQ(type->SecondConstant->Name(), L"Second");
    ASSERT_EQ(type->ThirdConstant->Name(), L"Third");
    ASSERT_EQ(type->OneConstant->Name(), L"One");
    ASSERT_EQ(type->TwoConstant->Name(), L"Two");
    ASSERT_EQ(type->ThreeConstant->Name(), L"Three");

    ASSERT_EQ(type->FirstConstant->ValueType(), type);
    ASSERT_TRUE(type->FirstConstant->Value()->IsEqual(*zaf::Box(TestType::First)));
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
    zaf::dynamic::Object object;

    ASSERT_THROW(parser->ParseFromAttribute(L"First", object), zaf::InvalidTypeError);

    auto node = zaf::XamlReader::FromString("<TestType>Second</TestType>")->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, object), zaf::InvalidTypeError);
}


TEST(EnumTest, SetValue) {

    auto type = TestTypeEnum::Type::Instance();

    {
        zaf::dynamic::Object object;
        ASSERT_THROW(type->SetValue(object, *type->FirstConstant->Value()), zaf::InvalidTypeError);
        ASSERT_THROW(type->SetValue(*type->CreateInstance(), object), zaf::InvalidTypeError);
    }

    {
        auto result = zaf::As<TestTypeEnum>(type->CreateInstance());
        type->SetValue(*result, *type->FirstConstant->Value());
        ASSERT_EQ(result->Value(), TestType::First);

        type->SetValue(*result, *type->ThirdConstant->Value());
        ASSERT_EQ(result->Value(), TestType::Third);
    }
}


TEST(EnumTest, CombineFlagValue) {

    //Not flags enum
    {
        auto type = TestTypeEnum::Type::Instance();
        ASSERT_THROW(
            type->CombineFlagValue(*type->CreateInstance(), *type->OneConstant->Value()), 
            zaf::InvalidOperationError);
    }

    auto type = FlagsTypeEnum::Type::Instance();

    //Invalid types
    {
        zaf::dynamic::Object object;
        ASSERT_THROW(
            type->CombineFlagValue(object, *type->Flag1Constant->Value()),
            zaf::InvalidTypeError);

        ASSERT_THROW(
            type->CombineFlagValue(*type->CreateInstance(), object), 
            zaf::InvalidTypeError);
    }

    {
        auto result = zaf::As<FlagsTypeEnum>(type->CreateInstance());
        type->CombineFlagValue(*result, *type->Flag1Constant->Value());
        ASSERT_EQ(result->Value(), FlagsType::Flag1);

        type->CombineFlagValue(*result, *type->Flag2Constant->Value());
        ASSERT_EQ(result->Value(), FlagsType::Flag1 | FlagsType::Flag2);

        type->CombineFlagValue(*result, *type->Flag3Constant->Value());
        ASSERT_EQ(result->Value(), FlagsType::Flag1 | FlagsType::Flag2 | FlagsType::Flag3);
    }
}