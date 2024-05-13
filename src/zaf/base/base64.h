#pragma once

/**
@file
    Defines functions related to Base64 encoding and decoding.
*/

#include <cstdint>
#include <string>
#include <string_view>
#include <zaf/base/byte_array.h>

namespace zaf {

/**
Encodes the specified data to a Base64-encoded string.

@param data
    A pointer to the data to be encoded. The data will be interpreted as a byte sequence.

@param size
    The size of the data to be encoded, in bytes.

@pre
    data is not null.

@return
    The Base64-encoded string. It will be an empty string if the size is zero.

@throw zaf::PreconditionError
    Thrown if the precondition is violated.

@throw std::bad_alloc
    Thrown if fails to allocate the required memory.
*/
std::wstring Base64Encode(const void* data, std::size_t size);


/**
Encodes the specified byte array to a Base64-encoded string.

@param byte_array
    The byte array to be encoded.

@return 
    The Base64-encoded string. It will be an empty string if byte_array is empty.

@throw std::bad_alloc
    Thrown if fails to allocate the required memory.
*/
std::wstring Base64Encode(const ByteArray& byte_array);


/**
Decodes the specified Base64-encoded string to raw data.

@param encoded_string
    The Base64-encoded string to be decoded.

@return 
    The decoded byte array. It will be an empty array if encoded_string is an empty string.

@throw zaf::InvalidDataError
    Thrown if encoded_string is not a well-formatted Base64-encoded string.

@throw std::bad_alloc
    Thrown if fails to allocate the required memory.
*/
ByteArray Base64Decode(std::wstring_view encoded_string);

}