#include <gtest/gtest.h>
#include <zaf/application.h>
#include "unit_test_delegate.h"

int main(int argc, char **argv) {

    auto delegate = std::make_shared<zaf::testing::UnitTestDelegate>();
    zaf::Application::Instance().Initialize({ .delegate = delegate });

    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    //Call Terminate() to destroy all windows created during testing.
    zaf::Application::Instance().Terminate();
    return result;
}