#include <deque>
#include <list>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/erase.h>

namespace {

template<template<typename E, typename ...> class C>
bool TestEraseElement() {

	C<std::string> container{ "0", "1", "2", "3" };
	std::size_t erased_count = zaf::Erase(container, "0");

    if (erased_count != 1) {
        return false;
    }

    C<std::string> expected{ "1", "2", "3" };
    return container == expected;
}


template<template<typename E, typename ...> class C>
bool TestEraseElementWithPredicate() {

	C<std::string> container{ "0", "1", "2", "3" };
	std::size_t erased_count = zaf::EraseIf(container, [](const std::string& v) {
		return v == "0";
	});

    if (erased_count != 1) {
        return false;
    }

    C<std::string> expected{ "1", "2", "3" };
    return container == expected;
}


template<template<typename E, typename ...> class C>
bool TestEraseDuplicatedElements() {

    C<std::string> container{ "0", "1", "2", "3", "1", "2", "3" };
    std::size_t erased_count = zaf::EraseDuplicates(container);

    if (erased_count != 3) {
        return false;
    }

    C<std::string> expected{ "0", "1", "2", "3" };
    return container == expected;
}

}


TEST(Erase, Element) {

	ASSERT_TRUE(TestEraseElement<std::vector>());
	ASSERT_TRUE(TestEraseElement<std::list>());
	ASSERT_TRUE(TestEraseElement<std::deque>());
}


TEST(Erase, Predicate) {

	ASSERT_TRUE(TestEraseElementWithPredicate<std::vector>());
	ASSERT_TRUE(TestEraseElementWithPredicate<std::list>());
	ASSERT_TRUE(TestEraseElementWithPredicate<std::deque>());
}


TEST(Erase, EraseDuplicates) {

    ASSERT_TRUE(TestEraseDuplicatedElements<std::vector>());
    ASSERT_TRUE(TestEraseDuplicatedElements<std::deque>());
}