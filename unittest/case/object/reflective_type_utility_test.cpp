#include <gtest/gtest.h>
#include <zaf/object/reflective_type_utility.h>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

using namespace zaf;

static_assert(!IsReflectiveTypeV<int>);
static_assert(!IsReflectiveTypeV<std::string>);

static_assert(IsReflectiveTypeV<Object>);
static_assert(IsReflectiveTypeV<Control>);
static_assert(IsReflectiveTypeV<Window>);