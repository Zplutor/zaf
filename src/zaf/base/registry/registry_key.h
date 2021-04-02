#pragma once

#include <Windows.h>
#include <string>
#include "zaf/base/registry/registry_rights.h"

namespace zaf {

class RegistryKey {
public:
    RegistryKey() = default;

    explicit RegistryKey(HKEY handle) : handle_(handle)  { }

    ~RegistryKey();

    RegistryKey(const RegistryKey&) = delete;
    RegistryKey& operator=(const RegistryKey&) = delete;

    RegistryKey(RegistryKey&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    RegistryKey& operator=(RegistryKey&& other) {
        handle_ = other.handle_;
        other.handle_ = nullptr;
        return *this;
    }

    HKEY GetHandle() const {
        return handle_;
    }

    RegistryKey CreateSubKey(const std::wstring& sub_key) {
        return CreateSubKey(sub_key, RegistryRights::Read | RegistryRights::Write);
    }

    RegistryKey CreateSubKey(const std::wstring& sub_key, RegistryRights rights);
    
    RegistryKey OpenSubKey(const std::wstring& sub_key) {
        return OpenSubKey(sub_key, RegistryRights::Read);
    }

    RegistryKey OpenSubKey(const std::wstring& sub_key, RegistryRights rights);

    std::wstring GetStringValue(const std::wstring& name);
    void SetStringValue(const std::wstring& name, const std::wstring& value);
    
    std::uint32_t GetDWordValue(const std::wstring& name);
    void SetDWordValue(const std::wstring& name, std::uint32_t value);

    std::uint64_t GetQWordValue(const std::wstring& name);
    void SetQWordValue(const std::wstring& name, std::uint64_t value);

private:
    std::vector<BYTE> GetValue(const std::wstring& name, DWORD expected_type);
    void SetValue(const std::wstring& name, DWORD type, const void* data, DWORD data_size);

private:
    HKEY handle_{};
};

}