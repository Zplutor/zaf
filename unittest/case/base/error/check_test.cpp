#include <gtest/gtest.h>
#include <zaf/base/error/check.h>

namespace {

class TestObject {
public:
    ~TestObject() {
        //Make sure ZAF_EXPECT can be used in destructor.
        //If not, a warning will be generated.
        ZAF_EXPECT(false);
    }
};

}


TEST(CheckTest, Exception) {

    ASSERT_THROW(ZAF_EXPECT(false), std::logic_error);
    ASSERT_NO_THROW(ZAF_EXPECT(true));
}