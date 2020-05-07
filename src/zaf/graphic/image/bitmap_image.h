#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/reflection/reflection_object.h>

namespace zaf {

class BitmapImage : public Image, public ReflectionObject {
public:
    ZAF_DECLARE_REFLECTION_TYPE();

public:
    std::wstring GetUrl() const {
        return url_;
    }

    void SetUrl(const std::wstring& url);

    Size GetPixelSize(std::error_code& error_code) override;
    std::pair<float, float> GetResolution(std::error_code& error_code) override;
    RenderBitmap CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) override;

private:
    void CheckInitialize(std::error_code& error_code);
    std::filesystem::path GetPathFromUrl() const;

private:
    std::wstring url_;
    std::shared_ptr<Image> image_;
};

}