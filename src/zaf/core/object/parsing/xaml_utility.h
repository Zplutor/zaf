#pragma once

#include <optional>
#include <string>

namespace zaf {

class XamlNode;

std::optional<std::wstring> GetContentStringFromXamlNode(const XamlNode& node);

}