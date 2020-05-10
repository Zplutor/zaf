#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/reflection/reflection_object.h>

namespace zaf {

class BitmapImage : public Image, public ReflectionObject {
public:
    ZAF_DECLARE_REFLECTION_TYPE();

public:
    std::wstring GetUri() const {
        return uri_;
    }

    void SetUri(const std::wstring& uri);

    Size GetPixelSize(std::error_code& error_code) override;
    std::pair<float, float> GetResolution(std::error_code& error_code) override;
    RenderBitmap CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) override;

private:
    void CheckInitialize(std::error_code& error_code);

private:
    std::wstring uri_;
    std::shared_ptr<Image> image_;
};

}