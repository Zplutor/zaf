#pragma once

#include <cstddef>

namespace zaf {

class Range {
public:
	Range() : position(0), length(0) { }
	Range(std::size_t index, std::size_t length) : position(index), length(length) { }

public:
	std::size_t position;
	std::size_t length;
};

}