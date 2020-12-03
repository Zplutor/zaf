#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {

class InspectorPort {
public:
    virtual ~InspectorPort() = default;

    virtual void ControlAddChild(const std::shared_ptr<Control>& parent) = 0;

    virtual void ControlRemoveChild(
        const std::shared_ptr<Control>& parent, 
        std::size_t removed_index) = 0;
};

}