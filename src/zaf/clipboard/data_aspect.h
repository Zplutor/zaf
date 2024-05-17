#pragma once

/**
@file
    Defines the enum zaf::clipboard::DataAspect.
*/

#include <objidl.h>

namespace zaf::clipboard {

enum class DataAspect {
    Content = DVASPECT_CONTENT,
    Thumbnail = DVASPECT_THUMBNAIL,
    Icon = DVASPECT_ICON,
};

}