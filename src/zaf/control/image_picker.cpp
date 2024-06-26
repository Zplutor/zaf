#include <zaf/control/image_picker.h>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/graphic/image/image.h>

namespace zaf {
namespace {

class ConstantImagePicker {
public:
    ConstantImagePicker(const std::shared_ptr<Image>& image) : image_(image) {

    }

    std::shared_ptr<Image> operator()(const Control& control) {

        if (auto uri_image = As<URIImage>(image_)) {
            uri_image->ChangeDPI(control.GetDPI());
        }

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