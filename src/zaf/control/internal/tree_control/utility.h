#pragma once

#include <zaf/base/define.h>

namespace zaf::internal {

bool IsAncestorOf(const IndexPath& ancestor, const IndexPath& path);

}