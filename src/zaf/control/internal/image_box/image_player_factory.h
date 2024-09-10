#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <zaf/control/internal/image_box/image_player.h>
#include <zaf/graphic/image.h>
#include <zaf/graphic/wic/bitmap_decoder.h>

namespace zaf::internal {

class ImagePlayerFactory {
public:
    static std::unique_ptr<ImagePlayer> CreateWithImage(const std::shared_ptr<Image>& image);
    static std::unique_ptr<ImagePlayer> CreateWithDecoder(const wic::BitmapDecoder& bitmap_decoder);
    static std::unique_ptr<ImagePlayer> CreateWithFilePath(const std::filesystem::path& path);
    static std::unique_ptr<ImagePlayer> CreateWithURI(const std::wstring& uri, float dpi);

public:
    ImagePlayerFactory() = delete;
};

}