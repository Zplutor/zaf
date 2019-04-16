#include <gtest/gtest.h>
#include <zaf/base/container/utility/conversion.h>
#include <zaf/base/container/utility/map.h>

TEST(Conversion, MakeVector) {

    std::set<int> set{ 0, 1, 2, 3, 4, 5 };
    auto vector = zaf::MakeVector(set);

    std::vector<int> expected_vector{ 0, 1, 2, 3, 4, 5 };
    ASSERT_EQ(vector, expected_vector);
}


TEST(Conversion, MakeVectorWithMapView) {

    std::map<int, std::string> map{ { 0, "0" }, { 1, "1" }, { 2, "2" } };

    auto key_vector = zaf::MakeVector(zaf::Keys(map));
    std::vector<int> expected_key_vector{ 0, 1, 2 };
    ASSERT_EQ(key_vector, expected_key_vector);

    auto value_vector = zaf::MakeVector(zaf::Values(map));
    std::vector<std::string> expected_value_vector{ "0", "1", "2" };
    ASSERT_EQ(value_vector, expected_value_vector);
}


TEST(Conversion, MakeList) {

    std::vector<int> vector{ 0, 1, 2, 3 };

    auto list = zaf::MakeList(vector);
    std::list<int> expected_vector{ 0, 1, 2, 3 };
    ASSERT_EQ(list, expected_vector);
}


TEST(Conversion, MakeSet) {

    std::list<int> list{ 0, 1, 2, 3 };

    auto set = zaf::MakeSet(list);
    std::set<int> expected_set{ 0, 1, 2, 3 };
    ASSERT_EQ(set, expected_set);
}