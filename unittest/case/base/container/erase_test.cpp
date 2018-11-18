#include <deque>
#include <list>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/erase.h>

namespace internal {

template<template<typename E, typename ...> class C>
bool TestEraseElement() {

	C<std::string> container{ "0", "1", "2", "3" };
	zaf::Erase(container, "0");

	if (*(container.begin()) == "0") {
		return false;
	}
	return true;
}


template<template<typename E, typename ...> class C>
bool TestEraseElementWithPredicate() {

	C<std::string> container{ "0", "1", "2", "3" };
	zaf::EraseIf(container, [](const std::string& v) {
		return v == "0";
	});

	if (*(container.begin()) == "0") {
		return false;
	}
	return true;
}

}


TEST(Erase, Element) {

	ASSERT_TRUE(internal::TestEraseElement<std::vector>());
	ASSERT_TRUE(internal::TestEraseElement<std::list>());
	ASSERT_TRUE(internal::TestEraseElement<std::deque>());
}


TEST(Erase, Predicate) {

	ASSERT_TRUE(internal::TestEraseElementWithPredicate<std::vector>());
	ASSERT_TRUE(internal::TestEraseElementWithPredicate<std::list>());
	ASSERT_TRUE(internal::TestEraseElementWithPredicate<std::deque>());
}