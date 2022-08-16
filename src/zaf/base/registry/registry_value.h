#pragma once

#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/base/registry/registry_value_type.h>

namespace zaf {

class RegistryValue : NonCopyable {
public:
    RegistryValue() = default;
    RegistryValue(RegistryValueType type, std::vector<std::byte> buffer);

    RegistryValue(RegistryValue&& other);
    RegistryValue& operator=(RegistryValue&& other);

    RegistryValueType Type() const {
        return type_;
    }

    const std::vector<std::byte>& Buffer() const {
        return buffer_;
    }

    std::wstring ToString() const;
    std::uint32_t ToDWord() const;
    std::uint64_t ToQWord() const;

private:
    RegistryValueType type_{ RegistryValueType::None };
    std::vector<std::byte> buffer_;
};

}