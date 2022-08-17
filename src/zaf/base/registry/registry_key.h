#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/base/registry/registry_rights.h>
#include <zaf/base/registry/registry_value.h>

namespace zaf {

class RegistryKey : NonCopyable {
public:
    RegistryKey() = default;
    explicit RegistryKey(HKEY handle) : handle_(handle)  { }

    ~RegistryKey();

    RegistryKey(RegistryKey&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    RegistryKey& operator=(RegistryKey&& other) {
        handle_ = other.handle_;
        other.handle_ = nullptr;
        return *this;
    }

    HKEY Handle() const {
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

    void DeleteValue(const std::wstring& name);

    RegistryValue GetValue(const std::wstring& name) const;

    std::wstring GetStringValue(const std::wstring& name);
    void SetStringValue(const std::wstring& name, const std::wstring& value);

    std::vector<std::wstring> GetMultiStringValue(const std::wstring& name);
    void SetMultiStringValue(const std::wstring& name, const std::vector<std::wstring>& value);
    
    std::uint32_t GetDWordValue(const std::wstring& name);
    void SetDWordValue(const std::wstring& name, std::uint32_t value);

    std::uint64_t GetQWordValue(const std::wstring& name);
    void SetQWordValue(const std::wstring& name, std::uint64_t value);

private:
    RegistryValue InnerGetValue(const std::wstring& name, DWORD flags) const;
    void SetValue(const std::wstring& name, DWORD type, const void* data, DWORD data_size);

private:
    HKEY handle_{};
};

}