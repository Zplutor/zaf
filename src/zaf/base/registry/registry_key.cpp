#include "zaf/base/registry/registry_key.h"
#include "zaf/base/error.h"

namespace zaf {

RegistryKey::~RegistryKey() {

    if (handle_ == nullptr) {
        return;
    }

    if ((handle_ == HKEY_CLASSES_ROOT) ||
        (handle_ == HKEY_CURRENT_CONFIG) ||
        (handle_ == HKEY_CURRENT_USER) ||
        (handle_ == HKEY_CURRENT_USER_LOCAL_SETTINGS) ||
        (handle_ == HKEY_LOCAL_MACHINE) ||
        (handle_ == HKEY_PERFORMANCE_DATA) ||
        (handle_ == HKEY_PERFORMANCE_NLSTEXT) ||
        (handle_ == HKEY_PERFORMANCE_TEXT) ||
        (handle_ == HKEY_USERS)) {
        return;
    }

    RegCloseKey(handle_);
}


RegistryKey RegistryKey::CreateSubKey(const std::wstring& subKey, RegistryRights rights, std::error_code& error) {

    HKEY sub_key_handle{};
    LSTATUS result = RegCreateKeyEx(
        handle_,
        subKey.c_str(),
        0,
        nullptr, 
        0,
        static_cast<REGSAM>(rights),
        nullptr,
        &sub_key_handle, 
        nullptr);

    error = MakeSystemErrorCode(result);
    if (error) {
        return {};
    }
    return RegistryKey(sub_key_handle);
}


RegistryKey RegistryKey::OpenSubKey(const std::wstring& subKey, RegistryRights rights, std::error_code& error) {

    HKEY sub_key_handle{};
    LSTATUS result = RegOpenKeyEx(handle_, subKey.c_str(), 0, static_cast<REGSAM>(rights), &sub_key_handle);

    error = MakeSystemErrorCode(result);
    if (error) {
        return {};
    }
    return RegistryKey(sub_key_handle);
}


std::wstring RegistryKey::GetStringValue(const std::wstring& name, std::error_code& error) {

    auto buffer = GetValue(name, REG_SZ, error);
    if (error) {
        return {};
    }

    std::wstring value(reinterpret_cast<const wchar_t*>(buffer.data()), buffer.size() / sizeof(wchar_t));
    if (! value.empty()) {
        if (value[value.length() - 1] == L'\0') {
            value.resize(value.length() - 1);
        }
    }
    return value;
}


std::uint32_t RegistryKey::GetDWordValue(const std::wstring& name, std::error_code& error) {

    auto buffer = GetValue(name, REG_DWORD, error);
    if (error) {
        return 0;
    }

    std::uint32_t value = *reinterpret_cast<const std::uint32_t*>(buffer.data());
    return value;
}


std::uint64_t RegistryKey::GetQWordValue(const std::wstring& name, std::error_code& error) {

    auto buffer = GetValue(name, REG_QWORD, error);
    if (error) {
        return 0;
    }

    std::uint64_t value = *reinterpret_cast<const std::uint64_t*>(buffer.data());
    return value;
}


std::vector<BYTE> RegistryKey::GetValue(const std::wstring& name, DWORD expected_type, std::error_code& error) {

    DWORD value_type{};
    DWORD buffer_size = 128;
    std::vector<BYTE> buffer(buffer_size);
    LSTATUS result = RegQueryValueEx(handle_, name.c_str(), nullptr, &value_type, &buffer[0], &buffer_size);

    if (result == ERROR_MORE_DATA) {
        buffer.resize(buffer_size);
        result = RegQueryValueEx(handle_, name.c_str(), nullptr, &value_type, &buffer[0], &buffer_size);
    }

    error = MakeSystemErrorCode(result);
    if (! error) {
        if (expected_type != value_type) {
            error = MakeSystemErrorCode(ERROR_INVALID_DATATYPE);
        }
    }
    return buffer;
}


void RegistryKey::SetStringValue(const std::wstring& name, const std::wstring& value, std::error_code& error) {
    SetValue(
        name, 
        REG_SZ,
        value.c_str(),
        (value.length() + 1) * sizeof(wchar_t),
        error);
}


void RegistryKey::SetDWordValue(const std::wstring& name, std::uint32_t value, std::error_code& error) {
    SetValue(name, REG_DWORD, &value, sizeof(value), error);
}


void RegistryKey::SetQWordValue(const std::wstring& name, std::uint64_t value, std::error_code& error) {
    SetValue(name, REG_QWORD, &value, sizeof(value), error);
}


void RegistryKey::SetValue(const std::wstring& name, DWORD type, const void* data, DWORD data_size, std::error_code& error) {

    LSTATUS result = RegSetValueEx(handle_, name.c_str(), 0, type, reinterpret_cast<const BYTE*>(data), data_size);
    error = MakeSystemErrorCode(result);
}

}

