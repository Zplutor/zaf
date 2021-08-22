#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/base/error/basic_error.h>
#include "utility/assert.h"

namespace {

class Base {
public:
    virtual ~Base() = default;
};

class Derived : public Base {

};

class Other {
public:
    virtual ~Other() = default;
};

}


TEST(CastTest, CastPointer) {

    //Non-const pointer
    {
        std::unique_ptr<Base> object = std::make_unique<Derived>();
        Derived* derived = zaf::As<Derived>(object.get());
        ASSERT_NE(derived, nullptr);
    }

    //Const pointer
    {
        std::unique_ptr<const Base> const_object = std::make_unique<Derived>();
        const Derived* derived = zaf::As<Derived>(const_object.get());
        ASSERT_NE(derived, nullptr);
    }

    //Failed cast
    {
        auto other_object = std::make_unique<Other>();
        Derived* derived = zaf::As<Derived>(other_object.get());
        ASSERT_EQ(derived, nullptr);
    }
}


TEST(CastTest, CastReference) {

    //Non-const reference
    {
        std::unique_ptr<Base> object = std::make_unique<Derived>();
        static_assert(std::is_same_v<Derived&, decltype(zaf::As<Derived>(*object))>);
        Derived& derived = zaf::As<Derived>(*object);
    }

    //Const reference
    {
        std::unique_ptr<const Base> const_object = std::make_unique<Derived>();
        static_assert(std::is_same_v<const Derived&, decltype(zaf::As<Derived>(*const_object))>);
        const Derived& derived = zaf::As<Derived>(*const_object);
    }

    //Failed cast
    {
        Other other;
        ASSERT_THROW_ERRC(zaf::As<Derived>(other), zaf::BasicErrc::InvalidCast);
    }
}


TEST(CastTest, CastSharedPtr) {

    //Non-const
    {
        std::shared_ptr<Base> object = std::make_shared<Derived>();

        static_assert(
            std::is_same_v<std::shared_ptr<Derived>, 
            decltype(zaf::As<Derived>(object))>);

        std::shared_ptr<Derived> derived = zaf::As<Derived>(object);
        ASSERT_NE(derived, nullptr);
    }

    //Const
    {
        const std::shared_ptr<Base> const_object = std::make_shared<Derived>();

        static_assert(
            std::is_same_v<std::shared_ptr<Derived>,
            decltype(zaf::As<Derived>(const_object))>);

        std::shared_ptr<Derived> derived = zaf::As<Derived>(const_object);
        ASSERT_NE(derived, nullptr);
    }

    //Failed cast
    {
        std::shared_ptr<Other> other_object = std::make_shared<Other>();

        std::shared_ptr<Derived> derived = zaf::As<Derived>(other_object);
        ASSERT_EQ(derived, nullptr);
    }
}