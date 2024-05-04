#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>

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


TEST(CheckTest, PreconditionError) {

    ASSERT_THROW(ZAF_EXPECT(false), zaf::PreconditionError);
    ASSERT_NO_THROW(ZAF_EXPECT(true));
}