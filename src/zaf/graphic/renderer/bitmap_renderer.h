#pragma once

#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class BitmapRenderer : public Renderer {
public:
    BitmapRenderer() { }
    BitmapRenderer(ID2D1BitmapRenderTarget* handle) : Renderer(handle) { }

    Bitmap GetBitmap(std::error_code& error_code) const;

    Bitmap GetBitmap() const {
        std::error_code error_code;
        auto result = GetBitmap(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    ID2D1BitmapRenderTarget* GetHandle() const {
        return static_cast<ID2D1BitmapRenderTarget*>(__super::GetHandle());
    }
};

}