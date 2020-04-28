#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/graphic/image/wic/bitmap_source.h>
#include <zaf/graphic/renderer/renderer.h>

namespace zaf::internal {

class WicBitmap : public Image {
public:
    explicit WicBitmap(const wic::BitmapSource& wic_bitmap) : wic_bitmap_(wic_bitmap) {

    }


    Size GetPixelSize(std::error_code& error_code) override {
        return wic_bitmap_.GetSize(error_code);
    }


    std::pair<float, float> GetResolution(std::error_code& error_code) override {

        auto resolution = wic_bitmap_.GetResolution(error_code);
        return std::make_pair(
            static_cast<float>(resolution.first), 
            static_cast<float>(resolution.second));
    }


    RenderBitmap CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) override {
        return renderer.CreateBitmap(wic_bitmap_, error_code);
    }

private:
    wic::BitmapSource wic_bitmap_;
};

}