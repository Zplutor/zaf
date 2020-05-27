#pragma once

#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class BitmapRenderer : public Renderer {
public:
    BitmapRenderer() { }
    BitmapRenderer(ID2D1BitmapRenderTarget* handle) : Renderer(handle) { }

    RenderBitmap GetBitmap() const;

    ID2D1BitmapRenderTarget* GetHandle() const {
        return static_cast<ID2D1BitmapRenderTarget*>(__super::GetHandle());
    }
};

}