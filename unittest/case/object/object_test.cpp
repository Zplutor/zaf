#include <gtest/gtest.h>
#include <zaf/base/error/not_supported_error.h>
#include <zaf/object/object_type.h>
#include <zaf/object/type_declaration.h>
#include <zaf/object/type_definition.h>

namespace {

class DefaultConstructible : public zaf::Object {
public:
    ZAF_DECLARE_TYPE;

    DefaultConstructible() = default;
};

ZAF_DEFINE_TYPE(DefaultConstructible)
ZAF_DEFINE_TYPE_END


class NotDefaultConstructible : public zaf::Object {
public:
    ZAF_DECLARE_TYPE;

    NotDefaultConstructible(int) { }
};

ZAF_DEFINE_TYPE(NotDefaultConstructible)
ZAF_DEFINE_TYPE_END

}


TEST(ObjectTest, CreateInstance) {

    auto instance = DefaultConstructible::Type->CreateInstance();
    ASSERT_NE(instance, nullptr);

    auto default_constructible = std::dynamic_pointer_cast<DefaultConstructible>(instance);
    ASSERT_NE(default_constructible, nullptr);

    ASSERT_THROW(NotDefaultConstructible::Type->CreateInstance(), zaf::NotSupportedError);
}