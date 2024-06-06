#pragma once

#include <Objidl.h>

namespace zaf {

constexpr GUID IID_IMemoryStream = {
    0x356c5d55, 0xbb00, 0x433f, { 0x89, 0xb, 0x72, 0xd8, 0x26, 0x43, 0xbd, 0xd0 } 
};

class IMemoryStream : public IStream {
public:
    virtual const std::byte* GetMemory() = 0;
};

}