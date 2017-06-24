#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/base/error.h>

int main(int argc, char **argv) {

    std::error_code error_code;
    zaf::Application::GetInstance().Initialize(error_code);
    if (! zaf::IsSucceeded(error_code)) {
        return error_code.value();
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}