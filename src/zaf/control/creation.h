#pragma once

#include <memory>

namespace zaf {

template<typename ControlType, typename... ArgumentTypes>
const std::shared_ptr<ControlType> CreateControl(ArgumentTypes&&... arguments) {

	auto control = std::make_shared<ControlType>(arguments...);
    Control::UpdateGuard update_guard(*control);
	control->Initialize();
	return control;
}

}