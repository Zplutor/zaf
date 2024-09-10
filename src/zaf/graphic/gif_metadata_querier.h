#pragma once

#include <optional>
#include <zaf/graphic/wic/metadata_query_reader.h>

namespace zaf {

enum GifDisposal {
    Unspecified = 0,
    None = 1,
    RestoreToBackgroundColor = 2,
    RestoreToPrevious = 3,
};


class GifGlobalMetadataQuerier {
public:
    explicit GifGlobalMetadataQuerier(const wic::MetadataQueryReader& querier) :
        querier_(querier) {

    }

    std::uint16_t GetWidth() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/logscrdesc/Width");
    }

    std::uint16_t GetHeight() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/logscrdesc/Height");
    }

    bool HasGlobalColorTable() const {
        auto value = querier_.QueryMetadata<std::uint16_t>(L"/logscrdesc/GlobalColorTableFlag");
        return value != 0;
    }

    std::uint8_t GetBackgroundColorIndex() const {
        return querier_.QueryMetadata<std::uint8_t>(L"/logscrdesc/BackgroundColorIndex");
    }

    std::vector<std::uint8_t> GetApplicationExtensionIdentifier() const {
        return querier_.QueryMetadata<std::vector<std::uint8_t>>(L"/appext/Application");
    }

    std::vector<std::uint8_t> GetApplicationExtensionData() const {
        return querier_.QueryMetadata<std::vector<std::uint8_t>>(L"/appext/Data");
    }

    std::optional<std::size_t> GetLoopCount() const;

private:
    wic::MetadataQueryReader querier_;
};


class GifFrameMetadataQuerier {
public:
    explicit GifFrameMetadataQuerier(const wic::MetadataQueryReader& querier) :
        querier_(querier) {

    }

    GifDisposal GetDisposal() const {
        return static_cast<GifDisposal>(querier_.QueryMetadata<std::uint8_t>(L"/grctlext/Disposal"));
    }

    std::uint16_t GetDelay() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/grctlext/Delay");
    }

    std::uint16_t GetLeft() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Left");
    }

    std::uint16_t GetTop() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Top");
    }

    std::uint16_t GetWidth() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Width");
    }

    std::uint16_t GetHeight() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/imgdesc/Height");
    }

private:
    wic::MetadataQueryReader querier_;
};

}