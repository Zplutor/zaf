#pragma once

#include <zaf/base/optional.h>
#include <zaf/graphic/image/image_metadata_querier.h>

namespace zaf {

enum GifDisposal {
    Unspecified = 0,
    None = 1,
    RestoreToBackgroundColor = 2,
    RestoreToPrevious = 3,
};


class GifGlobalMetadataQuerier {
public:
    explicit GifGlobalMetadataQuerier(const ImageMetadataQuerier& querier) :
        querier_(querier) {

    }

    std::uint16_t GetWidth(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/logscrdesc/Width", error_code);
    }

    std::uint16_t GetWidth() const {
        std::error_code error_code;
        auto result = GetWidth(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint16_t GetHeight(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/logscrdesc/Height", error_code);
    }

    std::uint16_t GetHeight() const {
        std::error_code error_code;
        auto result = GetHeight(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    bool HasGlobalColorTable(std::error_code& error_code) const {
        auto value = querier_.QueryMetadata<std::uint16_t>(L"/logscrdesc/GlobalColorTableFlag", error_code);
        return value != 0;
    }

    bool HasGlobalColorTable() const {
        std::error_code error_code;
        auto result = HasGlobalColorTable(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint8_t GetBackgroundColorIndex(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint8_t>(L"/logscrdesc/BackgroundColorIndex", error_code);
    }

    std::uint8_t GetBackgroundColorIndex() const {
        std::error_code error_code;
        auto result = GetBackgroundColorIndex(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::vector<std::uint8_t> GetApplicationExtensionIdentifier(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::vector<std::uint8_t>>(L"/appext/Application", error_code);
    }

    std::vector<std::uint8_t> GetApplicationExtensionIdentifier() const {
        std::error_code error_code;
        auto result = GetApplicationExtensionIdentifier(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::vector<std::uint8_t> GetApplicationExtensionData(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::vector<std::uint8_t>>(L"/appext/Data", error_code);
    }

    std::vector<std::uint8_t> GetApplicationExtensionData() const {
        std::error_code error_code;
        auto result = GetApplicationExtensionData(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    zaf::optional<std::size_t> GetLoopCount() const;

private:
    ImageMetadataQuerier querier_;
};


class GifFrameMetadataQuerier {
public:
    explicit GifFrameMetadataQuerier(const ImageMetadataQuerier& querier) :
        querier_(querier) {

    }

    GifDisposal GetDisposal(std::error_code& error_code) const {
        return static_cast<GifDisposal>(querier_.QueryMetadata<std::uint8_t>(L"/grctlext/Disposal", error_code));
    }

    GifDisposal GetDisposal() const {
        std::error_code error_code;
        auto result = GetDisposal(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint16_t GetDelay(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/grctlext/Delay", error_code);
    }

    std::uint16_t GetDelay() const {
        std::error_code error_code;
        auto result = GetDelay(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint16_t GetLeft(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Left", error_code);
    }

    std::uint16_t GetLeft() const {
        std::error_code error_code;
        auto result = GetLeft(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint16_t GetTop(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Top", error_code);
    }

    std::uint16_t GetTop() const {
        std::error_code error_code;
        auto result = GetTop(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint16_t GetWidth(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Width", error_code);
    }

    std::uint16_t GetWidth() const {
        std::error_code error_code;
        auto result = GetWidth(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::uint16_t GetHeight(std::error_code& error_code) const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Height", error_code);
    }

    std::uint16_t GetHeight() const {
        std::error_code error_code;
        auto result = GetHeight(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

private:
    ImageMetadataQuerier querier_;
};

}