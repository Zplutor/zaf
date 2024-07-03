#pragma once

#include <zaf/object/enum_support.h>

namespace zaf {

enum class ImageLayout {

    None,
    Tile,
    Center,
    Stretch,
    Zoom,
};

ZAF_ENUM_BEGIN(ImageLayout)
ZAF_ENUM_CONSTANT(None)
ZAF_ENUM_CONSTANT(Tile)
ZAF_ENUM_CONSTANT(Center)
ZAF_ENUM_CONSTANT(Stretch)
ZAF_ENUM_CONSTANT(Zoom)
ZAF_ENUM_END;

}