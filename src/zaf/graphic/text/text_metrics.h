#pragma once

#include <cstdint>

namespace zaf {

class TextMetrics {
public:
	TextMetrics() : 
		left(0), 
		top(0), 
		width(0), 
		height(0), 
		width_including_trailing_whitespace(0),
		layout_width(0),
		layout_height(0),
		max_bidi_reordering_depth(0),
		line_count(0) {
	
	}

public:
	float  left;
	float  top;
	float  width;
	float  height;
	float  width_including_trailing_whitespace;
	float  layout_width;
	float  layout_height;
	std::uint32_t max_bidi_reordering_depth;
	std::uint32_t line_count;
};

}