#include <zaf/internal/graphic/utility.h>
#include <zaf/graphic/wic/imaging_factory.h>

namespace zaf::internal {

wic::BitmapDecoder CreateBitmapDecoderFromSteam(const Stream& stream) {

    auto wic_stream = wic::ImagingFactory::Instance().CreateStream();
    wic_stream.InitializeFromStream(stream);

    return wic::ImagingFactory::Instance().CreateDecoderFromStream(wic_stream);
}

}