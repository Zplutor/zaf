#pragma once

#include <zaf/base/error/check.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/registry/registry_value_type.h>

namespace zaf {

class RegistryValue : NonCopyable {
public:
    RegistryValue() = default;
    RegistryValue(
        RegistryValueType value_type,
        std::unique_ptr<std::uint8_t[]> buffer,
        std::size_t buffer_length)
        : 
        type_(value_type),
        buffer_(std::move(buffer)),
        buffer_length_(buffer_length) {

        ZAF_EXPECT(buffer_);
    }

    RegistryValue(RegistryValue&& other) : 
        type_(other.type_), 
        buffer_(std::move(other.buffer_)),
        buffer_length_(other.buffer_length_) {

        other.type_ = RegistryValueType::None;
        other.buffer_length_ = 0;
    }

    RegistryValue& operator=(RegistryValue&& other) {

        type_ = other.type_;
        buffer_ = std::move(other.buffer_);
        buffer_length_ = other.buffer_length_;

        other.type_ = RegistryValueType::None;
        other.buffer_length_ = 0;

        return *this;
    }

    RegistryValueType Type() const {
        return type_;
    }

    const std::uint8_t* Buffer() const {
        return buffer_.get();
    }

    std::size_t BufferLength() const {
        return buffer_length_;
    }

    std::wstring ToString() const;
    std::uint32_t ToDWord() const;
    std::uint64_t ToQWord() const;

private:
    RegistryValueType type_{ RegistryValueType::None };
    std::unique_ptr<std::uint8_t[]> buffer_;
    std::size_t buffer_length_{};
};

}