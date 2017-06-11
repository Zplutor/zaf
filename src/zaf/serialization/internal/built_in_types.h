#pragma once

#include <vector>

namespace zaf {

class SerializableTypeProperties;

namespace internal {

std::vector<SerializableTypeProperties> GetBuiltInTypeProperties();

}
}