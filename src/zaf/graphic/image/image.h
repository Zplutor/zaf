#pragma once

#include <zaf/graphic/render_bitmap.h>

namespace zaf {

class Image {
public:
    Image() = default;
    virtual ~Image() = default;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    virtual RenderBitmap CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) = 0;

    RenderBitmap CreateRenderBitmap(Renderer& renderer) {
        std::error_code error_code;
        auto result = CreateRenderBitmap(renderer, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}