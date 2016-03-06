#pragma once

#include <dwrite.h>

namespace zaf {

enum class FontStyle {

	Normal = DWRITE_FONT_STYLE_NORMAL,
	Italic = DWRITE_FONT_STYLE_ITALIC,
	Oblique = DWRITE_FONT_STYLE_OBLIQUE,
};

}