#include <zaf/graphic/image/image.h>

namespace zaf {

std::size_t Image::GetFrameCount(std::error_code& error_code) const {

    UINT count = 0;
    HRESULT result = handle_->GetFrameCount(&count);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return count;
    }
    else {
        return 0;
    }
}


const std::shared_ptr<Image::Frame> Image::GetFrame(std::size_t index, std::error_code& error_code) const {

    IWICBitmapFrameDecode* handle = nullptr;
    HRESULT result = handle_->GetFrame(index, &handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<Frame>(handle);
    }
    else {
        return nullptr;
    }
}

}