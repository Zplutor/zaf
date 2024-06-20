#pragma once

#include <memory>
#include <zaf/graphic/image/image.h>

namespace zaf {

class Control;

using ImagePicker = std::function<std::shared_ptr<Image>(const Control& control)>;

ImagePicker CreateImagePicker(const std::shared_ptr<Image>& image);

}