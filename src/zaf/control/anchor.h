#pragma once

#include <zaf/base/flag_enum.h>

namespace zaf {

enum class Anchor {

	None = 0,
	Left = 1 << 0,
	Top = 1 << 1,
	Right = 1 << 2,
	Bottom = 1 << 3,
};

ZAF_ENABLE_FLAG_ENUM(Anchor);

}