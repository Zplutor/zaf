#pragma once

#include <zaf/io/stream/stream.h>
#include <zaf/graphic/image/wic/bitmap_decoder.h>

namespace zaf::internal {

wic::BitmapDecoder CreateBitmapDecoderFromSteam(const Stream& stream);

}