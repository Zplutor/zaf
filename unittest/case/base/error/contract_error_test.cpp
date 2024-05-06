#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>

using namespace zaf;

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


TEST(ContractErrorTest, PreconditionError) {

    ASSERT_THROW(ZAF_EXPECT(false), zaf::PreconditionError);
    ASSERT_NO_THROW(ZAF_EXPECT(true));
}


//The site of the error should be the same as where ZAF_EXPECT() is called.
TEST(ContractErrorTest, ErrorInfo) {

    SourceSite site;
    int prior_line{};

    std::string message;

    try {
        prior_line = __LINE__;
        ZAF_EXPECT(1+1<2);
    }
    catch (const PreconditionError& error) {
        site = error.Site();
        message = error.what();
    }

    ASSERT_STREQ(site.GetFile(), __FILE__);
    ASSERT_EQ(site.GetLine(), prior_line + 1);
    ASSERT_EQ(message, "1+1<2");
}
