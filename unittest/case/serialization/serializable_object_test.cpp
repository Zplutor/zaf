#include <gtest/gtest.h>
#include <zaf/serialization/serializable_object.h>

class Type1 : public zaf::SerializableObject {
public:
    ZAF_DECLARE_TYPE_NAME();
};

ZAF_DEFINE_TYPE_NAME(Type1);


class Type2 : public zaf::SerializableObject {
public:
    ZAF_DECLARE_TYPE_NAME();
};

ZAF_DEFINE_SCOPED_TYPE_NAME(Type2, zaf);


class Type3 {
public:
    class Inner : public zaf::SerializableObject {
    public:
        ZAF_DECLARE_TYPE_NAME();
    };
};

ZAF_DEFINE_INNER_TYPE_NAME(Type3, Inner);


class Type4 {
public:
    class Inner : public zaf::SerializableObject {
    public:
        ZAF_DECLARE_TYPE_NAME();
    };
};

ZAF_DEFINE_SCOPED_INNER_TYPE_NAME(Type4, Inner, zaf);


TEST(SerializableObject, TypeName) {

    ASSERT_STREQ(Type1::TypeName, L"Type1");
    ASSERT_STREQ(Type2::TypeName, L"Type2.zaf");
    ASSERT_STREQ(Type3::Inner::TypeName, L"Inner.Type3");
    ASSERT_STREQ(Type4::Inner::TypeName, L"Inner.Type4.zaf");
}