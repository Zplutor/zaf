#include <gtest/gtest.h>
#include <zaf/base/base64.h>

namespace {

struct TestItem {
    std::string raw;
    std::string encoded;
};

std::vector<TestItem> g_test_items{
    //{ "", "" }, 
    { "a", "YQ==" },
};

}


TEST(Base64Test, Encode) {

    for (const auto& each_item : g_test_items) {
        auto encoded = zaf::Base64Encode(each_item.raw.data(), each_item.raw.length());
        ASSERT_EQ(encoded, each_item.encoded);
    }
}


TEST(Base64Test, Decode) {

    for (const auto& each_item : g_test_items) {

        auto decoded = zaf::Base64Decode(each_item.encoded);

        std::string_view decoded_string{
            reinterpret_cast<const char*>(decoded.data()),
            decoded.size()
        };
        ASSERT_EQ(decoded_string, each_item.raw);
    }
}