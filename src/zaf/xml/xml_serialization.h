#pragma once

#include <zaf/base/stream.h>
#include <zaf/xml/xml_serializable.h>

namespace zaf {

std::wstring XMLSerializeToText(const XMLSerializable& serializable);
void XMLDeserializeFromText(std::wstring_view text, XMLSerializable& serializable);

}