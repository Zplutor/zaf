#include <zaf/internal/control/color_picker_impl.h>
#include <zaf/base/as.h>
#include <zaf/control/style/if.h>
#include <zaf/graphic/color.h>

namespace zaf::internal {

void ColorPickerImpl::AddPickObject(std::shared_ptr<Object> object) {
    pick_objects_.push_back(std::move(object));
}


Color ColorPickerImpl::operator()(const Control& control) const {

    for (const auto& each_object : pick_objects_) {

        auto if_object = As<If>(each_object);
        if (if_object) {

            auto result = if_object->Evaluate(control);
            if (result) {
                return As<Color>(*result);
            }
        }
        else {
            return As<Color>(*each_object);
        }
    }

    return Color::Transparent();
}

}