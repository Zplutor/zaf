#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/color.h>

namespace zaf::internal {

class ColorPickerImpl {
public:
    void AddPickObject(std::shared_ptr<dynamic::Object> object);

    Color operator()(const Control& control) const;

private:
    std::vector<std::shared_ptr<dynamic::Object>> pick_objects_;
};

}