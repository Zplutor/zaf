#include <gtest/gtest.h>
#include <zaf/base/number.h>

template<typename Type>
bool TestConstruct(Type value, zaf::Number::Tag tag) {

    zaf::Number number(value);

    return 
        (number.GetTag() == tag) && 
        (number.GetValue<Type>() == value);
}

template<typename Type>
bool TestAssign(Type value, zaf::Number::Tag tag) {

    zaf::Number number;
    number = value;

    return
        (number.GetTag() == tag) &&
        (number.GetValue<Type>() == value);
}


TEST(Number, DefaultConstruction) {

    zaf::Number number;
    ASSERT_EQ(number.GetTag(), zaf::Number::Tag::SignedInteger);
    ASSERT_EQ(number.GetValue<int>(), 0);
}


TEST(Number, ConstructWithNumber) {

    ASSERT_TRUE(TestConstruct<char>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestConstruct<signed char>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestConstruct<short>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestConstruct<int>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestConstruct<long>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestConstruct<long long>(-11, zaf::Number::Tag::SignedInteger));

    ASSERT_TRUE(TestConstruct<unsigned char>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestConstruct<unsigned short>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestConstruct<unsigned int>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestConstruct<unsigned long>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestConstruct<unsigned long long>(11, zaf::Number::Tag::UnsignedInteger));

    ASSERT_TRUE(TestConstruct<float>(11.11f, zaf::Number::Tag::FloatingPoint));
    ASSERT_TRUE(TestConstruct<double>(11.11, zaf::Number::Tag::FloatingPoint));
}


TEST(Number, AssignWithNumber) {

    ASSERT_TRUE(TestAssign<char>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestAssign<signed char>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestAssign<short>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestAssign<int>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestAssign<long>(-11, zaf::Number::Tag::SignedInteger));
    ASSERT_TRUE(TestAssign<long long>(-11, zaf::Number::Tag::SignedInteger));

    ASSERT_TRUE(TestAssign<unsigned char>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestAssign<unsigned short>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestAssign<unsigned int>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestAssign<unsigned long>(11, zaf::Number::Tag::UnsignedInteger));
    ASSERT_TRUE(TestAssign<unsigned long long>(11, zaf::Number::Tag::UnsignedInteger));

    ASSERT_TRUE(TestAssign<float>(11.11f, zaf::Number::Tag::FloatingPoint));
    ASSERT_TRUE(TestAssign<double>(11.11, zaf::Number::Tag::FloatingPoint));
}