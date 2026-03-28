#include <gtest/gtest.h>
#include <zaf/application.h>

int main(int argc, char **argv) {

    zaf::Application::Instance().Initialize({});

    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    //Call Exit() to destroy all windows created during testing.
    zaf::Application::Instance().Exit();
    return result;
}