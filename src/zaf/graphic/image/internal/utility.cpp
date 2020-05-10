#include <zaf/graphic/image/internal/utility.h>
#include <zaf/application.h>
#include <zaf/graphic/image/wic/imaging_factory.h>

namespace zaf::internal {

wic::BitmapDecoder CreateBitmapDecoderFromSteam(const Stream& stream, std::error_code& error_code) {

    auto wic_stream = GetImagingFactory().CreateStream(error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    wic_stream.InitializeFromStream(stream, error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    auto decoder = GetImagingFactory().CreateDecoderFromStream(wic_stream, error_code);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return decoder;
}

}