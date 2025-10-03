#pragma once

#include <gtest/gtest.h>
#include <zaf/application.h>

namespace zaf::testing {

class WindowTest : public ::testing::Test {
protected:
    bool IsWindowRegistered(const std::shared_ptr<Window>& window) {
        return zaf::Application::Instance().IsWindowRegistered(window);
    }
};

}