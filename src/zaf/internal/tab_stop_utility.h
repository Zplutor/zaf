#pragma once

#include <memory>

namespace zaf {

class Control;

namespace internal {

const std::shared_ptr<Control> FindNextTabStopControl(const std::shared_ptr<Control>& control);

}
}