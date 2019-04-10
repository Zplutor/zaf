#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <gtest/gtest.h>
#include <zaf/base/container/utility/classify.h>

TEST(Classify, IsAssociativeContainer) {

    ASSERT_TRUE((zaf::IsAssociativeContainer<std::map<int, int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::multimap<int, int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::unordered_map<int, int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::unordered_multimap<int, int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::set<int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::multiset<int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::unordered_set<int>>::Value));
    ASSERT_TRUE((zaf::IsAssociativeContainer<std::unordered_multiset<int>>::Value));

    ASSERT_FALSE((zaf::IsAssociativeContainer<std::vector<int>>::Value));
    ASSERT_FALSE((zaf::IsAssociativeContainer<std::list<int>>::Value));
    ASSERT_FALSE((zaf::IsAssociativeContainer<std::deque<int>>::Value));
}


TEST(Classify, IsMapContainer) {

    ASSERT_TRUE((zaf::IsMapContainer<std::map<int, int>>::Value));
    ASSERT_TRUE((zaf::IsMapContainer<std::multimap<int, int>>::Value));
    ASSERT_TRUE((zaf::IsMapContainer<std::unordered_map<int, int>>::Value));
    ASSERT_TRUE((zaf::IsMapContainer<std::unordered_multimap<int, int>>::Value));

    ASSERT_FALSE((zaf::IsMapContainer<std::vector<int>>::Value));
    ASSERT_FALSE((zaf::IsMapContainer<std::list<int>>::Value));
    ASSERT_FALSE((zaf::IsMapContainer<std::deque<int>>::Value));
    ASSERT_FALSE((zaf::IsMapContainer<std::set<int>>::Value));
    ASSERT_FALSE((zaf::IsMapContainer<std::multiset<int>>::Value));
    ASSERT_FALSE((zaf::IsMapContainer<std::unordered_set<int>>::Value));
    ASSERT_FALSE((zaf::IsMapContainer<std::unordered_multiset<int>>::Value));
}