#include <gtest/gtest.h>
#include <zaf/object/reflective_type_utility.h>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

using namespace zaf;

static_assert(!IsReflectiveType<int>);
static_assert(!IsReflectiveType<std::string>);

static_assert(IsReflectiveType<Object>);
static_assert(IsReflectiveType<Control>);
static_assert(IsReflectiveType<Window>);