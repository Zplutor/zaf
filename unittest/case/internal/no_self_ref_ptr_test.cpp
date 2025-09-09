#include <gtest/gtest.h>
#include <zaf/base/range.h>
#include <zaf/internal/no_self_ref_ptr.h>
#include <zaf/dynamic/object.h>

using namespace zaf;
using namespace zaf::internal;

TEST(NoSelfRefPtrTest, DefaultConstruction) {

    NoSelfRefPtr<int> ptr;
    ASSERT_FALSE(ptr);
    ASSERT_EQ(ptr.ToSharedPtr(), nullptr);
}


TEST(NoSelfRefPtrTest, StoreSharedPtr) {

    dynamic::Object owner;

    auto range = Create<Range>();
    NoSelfRefPtr<Range> ptr{ range, &owner };

    ASSERT_TRUE(ptr);
    ASSERT_EQ(ptr.ToSharedPtr(), range);
}


TEST(NoSelfRefPtrTest, StoreWeakPtr) {

    auto owner = Create<dynamic::Object>();

    NoSelfRefPtr<dynamic::Object> ptr{ owner, owner.get() };

    ASSERT_TRUE(ptr);
    ASSERT_EQ(ptr.ToSharedPtr(), owner);

    owner.reset();
    ASSERT_FALSE(ptr);
}