#include <gtest/gtest.h>
#include <zaf/base/base64.h>

namespace {

struct TestItem {
    std::string raw;
    std::string encoded;
};

std::vector<TestItem> g_test_items{
    { "", "" }, 
    { "a", "YQ==" },
};

}


TEST(Base64Test, Encode) {

    for (const auto& each_item : g_test_items) {
        auto encoded = zaf::Base64Encode(each_item.raw.data(), each_item.raw.length());
        ASSERT_EQ(encoded, each_item.encoded);
    }
}