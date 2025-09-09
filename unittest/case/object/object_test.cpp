#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/dynamic/object.h>
#include <zaf/dynamic/object_type.h>

namespace {

class DefaultConstructible : public zaf::dynamic::Object {
public:
    ZAF_OBJECT;

    DefaultConstructible() = default;
};

ZAF_OBJECT_BEGIN(DefaultConstructible);
ZAF_OBJECT_END;
ZAF_OBJECT_IMPL(DefaultConstructible);


class NotDefaultConstructible : public zaf::dynamic::Object {
public:
    ZAF_OBJECT;

    NotDefaultConstructible(int) { }
};

ZAF_OBJECT_BEGIN(NotDefaultConstructible);
ZAF_OBJECT_END;
ZAF_OBJECT_IMPL(NotDefaultConstructible);

}


TEST(ObjectTest, CreateInstance) {

    auto instance = DefaultConstructible::StaticType()->CreateInstance();
    ASSERT_NE(instance, nullptr);

    auto default_constructible = std::dynamic_pointer_cast<DefaultConstructible>(instance);
    ASSERT_NE(default_constructible, nullptr);

    ASSERT_THROW(
        NotDefaultConstructible::StaticType()->CreateInstance(), 
        zaf::InvalidOperationError);
}