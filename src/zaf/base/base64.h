#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace zaf {

std::string Base64Encode(const void* data, std::size_t size);

std::vector<std::byte> Base64Decode(std::string_view encoded);

}