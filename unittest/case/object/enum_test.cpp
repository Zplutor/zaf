#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_definition.h>
#include "utility/assert.h"

namespace {

enum class TestType {
    First,
    Second,
    Third,
};

ZAF_DECLARE_ENUM(TestType)

ZAF_DEFINE_ENUM(TestType)
ZAF_DEFINE_ENUM_VALUE(First)
ZAF_DEFINE_ENUM_VALUE(Second)
ZAF_DEFINE_ENUM_VALUE(Third)
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

    ASSERT_EQ(zaf::Box(static_cast<TestType>(1374))->ToString(), L"1374");
}


TEST(EnumTest, FindValue) {

    auto object = TestTypeEnum::EnumType()->FindValue(L"First");
    ASSERT_NE(object, nullptr);
    auto value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(value, TestType::First);

    object = TestTypeEnum::EnumType()->FindValue(L"Second");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(value, TestType::Second);

    object = TestTypeEnum::EnumType()->FindValue(L"Third");
    ASSERT_NE(object, nullptr);
    value = zaf::Unbox<TestType>(object);
    ASSERT_EQ(value, TestType::Third);

    object = TestTypeEnum::EnumType()->FindValue(L"Fourth");
    ASSERT_EQ(object, nullptr);
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

    ASSERT_THROW_ERRC(
        parser->ParseFromAttribute(L"Fourth", enum_object),
        zaf::BasicErrc::InvalidValue);
}