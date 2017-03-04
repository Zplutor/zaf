#pragma once

#include <zaf/graphic/image/image_metadata_querier.h>

namespace zaf {

class GifGlobalMetadataQuerier {
public:
    explicit GifGlobalMetadataQuerier(const ImageMetadataQuerier& querier) :
        querier_(querier) {

    }



private:
    ImageMetadataQuerier querier_;
};


class GifFrameMetadataQuerier {
public:
    explicit GifFrameMetadataQuerier(const ImageMetadataQuerier& querier) :
        querier_(querier) {

    }

    std::uint16_t QueryDelay() const {
        return querier_.QueryMetadata<std::uint16_t>(L"/grctlext/Delay");
    }

private:
    ImageMetadataQuerier querier_;
};

}