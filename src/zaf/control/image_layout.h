#pragma once

#include <zaf/object/enum_declaration.h>
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
ZAF_ENUM_VALUE(None)
ZAF_ENUM_VALUE(Tile)
ZAF_ENUM_VALUE(Center)
ZAF_ENUM_VALUE(Stretch)
ZAF_ENUM_VALUE(Zoom)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(ImageLayout)

}