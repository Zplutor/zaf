#include <gtest/gtest.h>
#include <zaf/base/hash.h>
#include <zaf/object/boxing/boxed_object.h>
#include <zaf/object/boxing/boxing.h>

namespace {

class EqualComparableObject {
public:
    int value{};
};

bool operator==(const EqualComparableObject& o1, const EqualComparableObject& o2) {
    return o1.value == o2.value;
}

class UncomparableObject {
public:
    int value{};
};

}

namespace std {
template<>
struct hash<EqualComparableObject> {
    std::size_t operator()(const EqualComparableObject& object) {
        return zaf::CalculateHash(object.value);
    }
};
}

TEST(BoxedObjectEqualityTest, Comparable) {

    EqualComparableObject o1;
    o1.value = 11;

    EqualComparableObject o2;
    o2.value = 11;

    auto boxed1 = zaf::Box(o1);
    auto boxed2 = zaf::Box(o2);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_TRUE(boxed2->IsEqual(*boxed1));
    ASSERT_EQ(boxed1->Hash(), boxed2->Hash());
}


TEST(BoxedObjectEqualityTest, Uncomparable) {

    UncomparableObject o1;
    o1.value = 22;

    UncomparableObject o2;
    o2.value = 22;

    auto boxed1 = zaf::Box(o1);
    auto boxed2 = zaf::Box(o2);

    ASSERT_FALSE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed2->IsEqual(*boxed1));
    ASSERT_NE(boxed1->Hash(), boxed2->Hash());
}