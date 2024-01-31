#pragma once

#include <functional>
#include <zaf/window/window.h>

void TestWithWindow(const std::function<void(zaf::Window&)>& test);