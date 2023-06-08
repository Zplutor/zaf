#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace zaf {

std::string Base64Encode(const void* data, std::size_t size);

}