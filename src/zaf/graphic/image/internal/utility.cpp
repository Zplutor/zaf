#include <zaf/graphic/image/internal/utility.h>
#include <zaf/application.h>
#include <zaf/graphic/image/wic/imaging_factory.h>

namespace zaf::internal {

wic::BitmapDecoder CreateBitmapDecoderFromSteam(const Stream& stream) {

    auto wic_stream = GetImagingFactory().CreateStream();
    wic_stream.InitializeFromStream(stream);

    return GetImagingFactory().CreateDecoderFromStream(wic_stream);
}

}