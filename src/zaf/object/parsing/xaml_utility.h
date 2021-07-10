#pragma once

#include <string>

namespace zaf {

class XamlNode;

std::wstring GetContentStringFromXamlNode(const XamlNode& node);

}