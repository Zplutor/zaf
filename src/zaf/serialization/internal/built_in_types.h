#pragma once

#include <memory>
#include <vector>

namespace zaf {

class SerializableType;

namespace internal {

std::vector<std::shared_ptr<SerializableType>> GetBuiltInSerializableTypes();

}
}