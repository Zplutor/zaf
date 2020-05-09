#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/parsing/parsers/bitmap_image_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(BitmapImage, ReflectionObject);


void BitmapImage::SetUri(const std::wstring& uri) {

    if (!image_) {
        uri_ = uri;
    }
    else {
        ZAF_FAIL();
    }
}


Size BitmapImage::GetPixelSize(std::error_code& error_code) {

    CheckInitialize(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return image_->GetPixelSize(error_code);
}


std::pair<float, float> BitmapImage::GetResolution(std::error_code& error_code) {

    CheckInitialize(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return image_->GetResolution(error_code);
}


RenderBitmap BitmapImage::CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) {

    CheckInitialize(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return image_->CreateRenderBitmap(renderer, error_code);
}


void BitmapImage::CheckInitialize(std::error_code& error_code) {

    if (image_) {
        error_code.clear();
        return;
    }

    auto path = GetPathFromUri();
    if (path.empty()) {
        error_code = MakeSystemErrorCode(ERROR_FILE_NOT_FOUND);
        return;
    }

    image_ = Image::FromFile(path, error_code);
}


std::filesystem::path BitmapImage::GetPathFromUri() const {

    const std::wstring file_schema_prefix{ L"file:///" };

    if (uri_.find(file_schema_prefix) == 0) {

        std::filesystem::path path = uri_.substr(file_schema_prefix.length());
        if (path.is_absolute()) {
            return path;
        }
        else {
            return {};
        }
    }
    else {

        std::filesystem::path path = uri_;
        if (path.is_absolute()) {
            return path;
        }

        wchar_t buffer[MAX_PATH]{};
        GetModuleFileName(nullptr, buffer, MAX_PATH);

        path.assign(buffer);
        return path / uri_;
    }
}

}