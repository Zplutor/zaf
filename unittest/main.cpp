#include <gtest/gtest.h>
#include <zaf/application.h>

int main(int argc, char **argv) {

    zaf::Application::GetInstance().Initialize({});

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}