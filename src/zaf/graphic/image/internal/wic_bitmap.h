#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/graphic/image/wic/bitmap_source.h>
#include <zaf/graphic/d2d/renderer.h>

namespace zaf::internal {

class WicBitmap : public Image {
public:
    explicit WicBitmap(const wic::BitmapSource& wic_bitmap) : wic_bitmap_(wic_bitmap) {

    }


    Size GetPixelSize() override {
        return wic_bitmap_.GetSize();
    }


    std::pair<float, float> GetResolution() override {

        auto resolution = wic_bitmap_.GetResolution();
        return std::make_pair(
            static_cast<float>(resolution.first), 
            static_cast<float>(resolution.second));
    }


    d2d::RenderBitmap CreateRenderBitmap(d2d::Renderer& renderer) override {
        return renderer.CreateBitmap(wic_bitmap_);
    }

private:
    wic::BitmapSource wic_bitmap_;
};

}