#pragma once

#include <zaf/object/enum_declaration.h>

namespace zaf {

enum class ImageLayout {

    None,
    Tile,
    Center,
    Stretch,
    Zoom,
};

ZAF_DECLARE_ENUM(ImageLayout)
ZAF_ENABLE_ENUM_BOXING(ImageLayout)

}