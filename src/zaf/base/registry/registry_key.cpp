#include <zaf/base/registry/registry_key.h>
#include <zaf/base/error/system_error.h>

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


RegistryKey RegistryKey::CreateSubKey(const std::wstring& sub_key, RegistryRights rights) {

    HKEY sub_key_handle{};
    LSTATUS result = RegCreateKeyEx(
        handle_,
        sub_key.c_str(),
        0,
        nullptr, 
        0,
        static_cast<REGSAM>(rights),
        nullptr,
        &sub_key_handle, 
        nullptr);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
    return RegistryKey{ sub_key_handle };
}


RegistryKey RegistryKey::OpenSubKey(const std::wstring& sub_key, RegistryRights rights) {

    HKEY sub_key_handle{};
    LSTATUS result = RegOpenKeyEx(
        handle_, 
        sub_key.c_str(),
        0, 
        static_cast<REGSAM>(rights),
        &sub_key_handle);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
    return RegistryKey{ sub_key_handle };
}


void RegistryKey::DeleteValue(const std::wstring& name) {

    LSTATUS result = RegDeleteValue(handle_, name.c_str());
    ZAF_THROW_IF_SYSTEM_ERROR(result);
}


std::wstring RegistryKey::GetStringValue(const std::wstring& name) {

    auto buffer = GetValue(name, REG_SZ);

    std::wstring value{ 
        reinterpret_cast<const wchar_t*>(buffer.data()),
        buffer.size() / sizeof(wchar_t) 
    };

    if (! value.empty()) {
        if (value[value.length() - 1] == L'\0') {
            value.resize(value.length() - 1);
        }
    }
    return value;
}


void RegistryKey::SetStringValue(const std::wstring& name, const std::wstring& value) {
    SetValue(name, REG_SZ, value.c_str(), (value.length() + 1) * sizeof(wchar_t));
}


std::uint32_t RegistryKey::GetDWordValue(const std::wstring& name) {

    auto buffer = GetValue(name, REG_DWORD);

    std::uint32_t value = *reinterpret_cast<const std::uint32_t*>(buffer.data());
    return value;
}


void RegistryKey::SetDWordValue(const std::wstring& name, std::uint32_t value) {
    SetValue(name, REG_DWORD, &value, sizeof(value));
}


std::uint64_t RegistryKey::GetQWordValue(const std::wstring& name) {

    auto buffer = GetValue(name, REG_QWORD);

    std::uint64_t value = *reinterpret_cast<const std::uint64_t*>(buffer.data());
    return value;
}


void RegistryKey::SetQWordValue(const std::wstring& name, std::uint64_t value) {
    SetValue(name, REG_QWORD, &value, sizeof(value));
}


std::vector<BYTE> RegistryKey::GetValue(const std::wstring& name, DWORD expected_type) {

    DWORD value_type{};
    DWORD buffer_size{ 128 };
    std::vector<BYTE> buffer(buffer_size);
    LSTATUS result = RegQueryValueEx(
        handle_, 
        name.c_str(),
        nullptr, 
        &value_type, 
        &buffer[0],
        &buffer_size);

    if (result == ERROR_MORE_DATA) {
        buffer.resize(buffer_size);
        result = RegQueryValueEx(handle_, name.c_str(), nullptr, &value_type, &buffer[0], &buffer_size);
    }

    ZAF_THROW_IF_SYSTEM_ERROR(result);

    if (expected_type != value_type) {
        ZAF_THROW_IF_SYSTEM_ERROR(ERROR_INVALID_DATATYPE);
    }
    return buffer;
}


void RegistryKey::SetValue(
    const std::wstring& name,
    DWORD type, 
    const void* data,
    DWORD data_size) {

    LSTATUS result = RegSetValueEx(
        handle_, 
        name.c_str(),
        0, 
        type,
        reinterpret_cast<const BYTE*>(data),
        data_size);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
}

}

