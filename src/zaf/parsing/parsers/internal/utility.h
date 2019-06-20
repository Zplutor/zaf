#pragma once

#include <string>

namespace zaf {
namespace internal {

bool ParseAttributeToDoubleFloats(
    const std::wstring& attribute, 
    float& float0,
    float& float1);

bool ParseAttributeToQuaterFloats(
    const std::wstring& attribute, 
    float& float0,
    float& float1,
    float& float2,
    float& float3);

}
}