#pragma once

#include <memory>
#include <zaf/graphic/image.h>

namespace zaf {

class Control;

using ImagePicker = std::function<std::shared_ptr<Image>(const Control& control)>;

ImagePicker CreateImagePicker(const std::shared_ptr<Image>& image);

}