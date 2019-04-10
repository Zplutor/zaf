#include <map>
#include <unordered_map>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/find.h>

namespace internal {

template<template<typename K, typename V, typename ...> class C>
bool TestMap() {

	C<int, std::string> map{ { 0, "0" }, { 1, "1" }, { 2, "2" } };
	auto value = zaf::Find(map, 0);
	if (value == nullptr || *value != "0") {
		return false;
	}

	value = zaf::Find(map, 4);
	if (value != nullptr) {
		return false;
	}

	// Test const overload
	const C<int, std::string>& const_map = map;
	auto const_value = zaf::Find(const_map, 1);
	if (const_value == nullptr || *const_value != "1") {
		return false;
	}

	return true;
}

}

TEST(Find, Map) {

	ASSERT_TRUE(internal::TestMap<std::map>());
	ASSERT_TRUE(internal::TestMap<std::multimap>());
	ASSERT_TRUE(internal::TestMap<std::unordered_map>());
	ASSERT_TRUE(internal::TestMap<std::unordered_multimap>());
}