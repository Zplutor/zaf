#include <zaf/base/registry/registry_key.h>
#include <zaf/base/error/system_error.h>

namespace zaf {

RegistryKey::~RegistryKey() {

    if (!handle_) {
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


RegistryValue RegistryKey::GetValue(const std::wstring& name) const {
    return InnerGetValue(name, RRF_RT_ANY);
}


std::wstring RegistryKey::GetStringValue(const std::wstring& name) {
    return InnerGetValue(name, RRF_RT_REG_SZ).ToString();
}


void RegistryKey::SetStringValue(const std::wstring& name, const std::wstring& value) {
    SetValue(
        name,
        REG_SZ,
        value.c_str(),
        static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
}


std::uint32_t RegistryKey::GetDWordValue(const std::wstring& name) {
    return InnerGetValue(name, RRF_RT_REG_DWORD).ToDWord();
}


void RegistryKey::SetDWordValue(const std::wstring& name, std::uint32_t value) {
    SetValue(name, REG_DWORD, &value, sizeof(value));
}


std::uint64_t RegistryKey::GetQWordValue(const std::wstring& name) {
    return InnerGetValue(name, RRF_RT_REG_QWORD).ToQWord();
}


void RegistryKey::SetQWordValue(const std::wstring& name, std::uint64_t value) {
    SetValue(name, REG_QWORD, &value, sizeof(value));
}


RegistryValue RegistryKey::InnerGetValue(const std::wstring& name, DWORD flags) const {

    DWORD value_type{};
    DWORD buffer_size{ 128 };
    std::vector<std::byte> buffer(buffer_size);
    LSTATUS result = RegGetValue(
        handle_,
        nullptr,
        name.c_str(),
        flags,
        &value_type,
        reinterpret_cast<BYTE*>(&buffer[0]),
        &buffer_size);

    if (result == ERROR_MORE_DATA) {

        buffer.resize(buffer_size);
        result = RegGetValue(
            handle_,
            nullptr,
            name.c_str(),
            flags,
            &value_type,
            reinterpret_cast<BYTE*>(&buffer[0]),
            &buffer_size);
    }

    ZAF_THROW_IF_SYSTEM_ERROR(result);

    buffer.resize(buffer_size);

    return RegistryValue{
        static_cast<RegistryValueType>(value_type),
        std::move(buffer)
    };
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

