#pragma once

#include <dwrite.h>
#include <cstdint>

namespace zaf {

class LineMetrics {
public:
	LineMetrics() : 
		length(0),
		trailing_whitespace_length(0),
		newline_length(0),
		height(0),
		baseline(0),
		is_trimmed(false) {
	
	}

	std::uint32_t length;
	std::uint32_t trailing_whitespace_length;
	std::uint32_t newline_length;
	float height;
	float baseline;
	bool is_trimmed;
};

}