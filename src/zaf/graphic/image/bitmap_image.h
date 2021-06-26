#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/object/object.h>

namespace zaf {

class BitmapImage : public Image, public Object {
public:
    ZAF_DECLARE_TYPE

public:
    std::wstring GetUri() const {
        return uri_;
    }

    void SetUri(const std::wstring& uri);

    Size GetPixelSize() override;
    std::pair<float, float> GetResolution() override;
    RenderBitmap CreateRenderBitmap(Renderer& renderer) override;

private:
    void CheckInitialize();

private:
    std::wstring uri_;
    std::shared_ptr<Image> image_;
};

}