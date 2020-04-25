#include <zaf/control/image_picker.h>

namespace zaf {
namespace {

class ConstantImagePicker {
public:
    ConstantImagePicker(const std::shared_ptr<Image>& image) : image_(image) {

    }

    std::shared_ptr<Image> operator()(const Control&) {
        return image_;
    }

private:
    std::shared_ptr<Image> image_;
};

}

ImagePicker CreateImagePicker(const std::shared_ptr<Image>& image) {
    return ConstantImagePicker{ image };
}

}