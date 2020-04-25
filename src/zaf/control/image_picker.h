#pragma once

#include <memory>

namespace zaf {

class Control;
class Image;

using ImagePicker = std::function<std::shared_ptr<Image>(const Control& control)>;

ImagePicker CreateImagePicker(const std::shared_ptr<Image>& image);

}