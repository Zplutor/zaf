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

	C<int> container{ 3, 3, 3, 1, 1, 2, 2, 0 };

	auto new_container = zaf::ToUnique(container);
	if (!CheckElementOrder(new_container)) {
		return false;
	}

	zaf::Unique(container);
	return CheckElementOrder(container);
}

}

TEST(Unique, Normal) {

	ASSERT_TRUE(internal::TestUnique<std::vector>());
	ASSERT_TRUE(internal::TestUnique<std::deque>());
}