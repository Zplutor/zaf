#include <vector>
#include <list>
#include <deque>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/unique.h>

namespace internal {

template<typename C>
bool CheckElementOrder(const C& container) {
	int index = 0;
	for (int each_value : container) {
		if (each_value != index) {
			return false;
		}
		index++;
	}
	return true;
}

template<template<typename E, typename ...> class C>
bool TestUnique() {

	C<int> container{ 0, 1, 1, 2, 2, 3, 3, 3 };

	zaf::Unique(container);
	return CheckElementOrder(container);
}

}

TEST(Unique, Normal) {

	ASSERT_TRUE(internal::TestUnique<std::vector>());
    ASSERT_TRUE(internal::TestUnique<std::list>());
	ASSERT_TRUE(internal::TestUnique<std::deque>());
}