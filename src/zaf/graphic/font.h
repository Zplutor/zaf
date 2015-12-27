#pragma once

#include <string>

namespace zaf {

class Font {
public:
	Font(const std::wstring& family_name, float size) : 
		family_name_(family_name), 
		size_(size) {

	}

	const std::wstring& GetFamilyName() const {
		return family_name_;
	}

	float GetSize() const {
		return size_;
	}

private:
	std::wstring family_name_;
	float size_;
};

}