#include <deque>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/sort.h>

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
bool TestSort() {

	C<int> container{ 4, 3, 2, 1, 0 };

	auto new_container = zaf::MakeSorted(container);
	if (!CheckElementOrder(new_container)) {
		return false;
	}

	zaf::Sort(container);
	return CheckElementOrder(container);
}


template<template<typename E, typename ...> class C>
bool TestSortWithPredicate() {

	C<int> container{ 4, 3, 2, 1, 0 };

    auto new_container = zaf::MakeSorted(container, [](int v1, int v2) {
        return v1 < v2;
    });
	if (!CheckElementOrder(new_container)) {
		return false;
	}

	zaf::Sort(container, [](int v1, int v2) {
		return v1 < v2;
	});
	return CheckElementOrder(container);
}

}

TEST(Sort, Normal) {

	ASSERT_TRUE(internal::TestSort<std::vector>());
	ASSERT_TRUE(internal::TestSort<std::deque>());
}


TEST(Sort, Predicate) {

	ASSERT_TRUE(internal::TestSortWithPredicate<std::vector>());
	ASSERT_TRUE(internal::TestSortWithPredicate<std::deque>());
}