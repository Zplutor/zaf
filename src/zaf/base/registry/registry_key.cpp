#include <zaf/base/registry/registry_key.h>
#include <zaf/base/error/check.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/string/join.h>

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


RegistryKey RegistryKey::InnerCreateSubKey(
    const std::wstring& sub_key,
    RegistryRights rights,
    std::optional<RegistryView> view) {

    auto new_key_view = GetViewForSubKey(view);

    HKEY sub_key_handle{};
    LSTATUS result = RegCreateKeyEx(
        handle_,
        sub_key.c_str(),
        0,
        nullptr,
        0,
        static_cast<REGSAM>(rights) | static_cast<REGSAM>(new_key_view),
        nullptr,
        &sub_key_handle,
        nullptr);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
    return RegistryKey{ sub_key_handle, new_key_view };
}


RegistryKey RegistryKey::InnerOpenSubKey(
    const std::wstring& sub_key,
    RegistryRights rights,
    std::optional<RegistryView> view) {

    auto new_key_view = GetViewForSubKey(view);

    HKEY sub_key_handle{};
    LSTATUS result = RegOpenKeyEx(
        handle_, 
        sub_key.c_str(),
        0, 
        static_cast<REGSAM>(rights) | static_cast<REGSAM>(new_key_view),
        &sub_key_handle);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
    return RegistryKey{ sub_key_handle, new_key_view };
}


void RegistryKey::InnerDeleteSubKey(
    const std::wstring& sub_key, 
    std::optional<RegistryView> view) {

    auto sub_key_view = GetViewForSubKey(view);

    LSTATUS result = RegDeleteKeyEx(
        handle_, 
        sub_key.c_str(),
        static_cast<REGSAM>(sub_key_view),
        0);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
}


void RegistryKey::DeleteValue(const std::wstring& name) {

    LSTATUS result = RegDeleteValue(handle_, name.c_str());
    ZAF_THROW_IF_SYSTEM_ERROR(result);
}


RegistryValue RegistryKey::InnerGetGenericValue(
    const std::wstring& sub_key,
    const std::wstring& name,
    std::optional<RegistryView> view) const {

    return InnerGetValue(sub_key, name, RRF_RT_ANY, view);
}


std::wstring RegistryKey::InnerGetStringValue(
    const std::wstring& sub_key, 
    const std::wstring& name,
    std::optional<RegistryView> view) const {

    return InnerGetValue(sub_key, name, RRF_RT_REG_SZ, view).ToString();
}


std::wstring RegistryKey::InnerGetExpandableStringValue(
    const std::wstring& sub_key, 
    const std::wstring& name,
    std::optional<RegistryView> view) const {

    auto value = InnerGetValue(sub_key, name, RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, view);
    return value.ToExpandableString();
}


std::vector<std::wstring> RegistryKey::InnerGetMultiStringValue(
    const std::wstring& sub_key, 
    const std::wstring& name,
    std::optional<RegistryView> view) const {

    return InnerGetValue(sub_key, name, RRF_RT_REG_MULTI_SZ, view).ToMultiString();
}


std::uint32_t RegistryKey::InnerGetDWordValue(
    const std::wstring& sub_key, 
    const std::wstring& name,
    std::optional<RegistryView> view) const {

    return InnerGetValue(sub_key, name, RRF_RT_REG_DWORD, view).ToDWord();
}


std::uint64_t RegistryKey::InnerGetQWordValue(
    const std::wstring& sub_key, 
    const std::wstring& name,
    std::optional<RegistryView> view) const {

    return InnerGetValue(sub_key, name, RRF_RT_REG_QWORD, view).ToQWord();
}


RegistryValue RegistryKey::InnerGetValue(
    const std::wstring& sub_key, 
    const std::wstring& name, 
    DWORD expected_value_type,
    std::optional<RegistryView> view) const {

    DWORD flags = expected_value_type;

    if (!sub_key.empty()) {

        auto sub_key_view = GetViewForSubKey(view);
        if (sub_key_view == zaf::RegistryView::Registry32) {
            flags |= RRF_SUBKEY_WOW6432KEY;
        }
        else if (sub_key_view == zaf::RegistryView::Registry64) {
            flags |= RRF_SUBKEY_WOW6464KEY;
        }
    }

    DWORD value_type{};
    DWORD buffer_size{ 128 };
    std::vector<std::byte> buffer(buffer_size);
    LSTATUS result = RegGetValue(
        handle_,
        sub_key.c_str(),
        name.c_str(),
        flags,
        &value_type,
        reinterpret_cast<BYTE*>(&buffer[0]),
        &buffer_size);

    if (result == ERROR_MORE_DATA) {

        buffer.resize(buffer_size);
        result = RegGetValue(
            handle_,
            sub_key.c_str(),
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


void RegistryKey::SetStringValue(
    const std::wstring& sub_key, 
    const std::wstring& name, 
    const std::wstring& value) {

    InnerSetValue(
        sub_key,
        name,
        REG_SZ,
        value.c_str(),
        static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
}


void RegistryKey::SetExpandableStringValue(
    const std::wstring& sub_key,
    const std::wstring& name, 
    const std::wstring& value) {

    InnerSetValue(
        sub_key,
        name,
        REG_EXPAND_SZ,
        value.c_str(),
        static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
}


void RegistryKey::SetMultiStringValue(
    const std::wstring& sub_key,
    const std::wstring& name, 
    const std::vector<std::wstring>& value) {

    std::wstring data;
    for (const auto& each_string : value) {
        data.append(each_string).append(1, L'\0');
    }
    data.append(1, L'\0');

    InnerSetValue(
        sub_key,
        name,
        REG_MULTI_SZ,
        data.data(),
        static_cast<DWORD>(data.size() * sizeof(wchar_t)));
}


void RegistryKey::SetDWordValue(
    const std::wstring& sub_key,
    const std::wstring& name,
    std::uint32_t value) {

    InnerSetValue(sub_key, name, REG_DWORD, &value, sizeof(value));
}


void RegistryKey::SetQWordValue(
    const std::wstring& sub_key,
    const std::wstring& name, 
    std::uint64_t value) {

    InnerSetValue(sub_key, name, REG_QWORD, &value, sizeof(value));
}


void RegistryKey::InnerSetValue(
    const std::wstring& sub_key,
    const std::wstring& name,
    DWORD type, 
    const void* data,
    DWORD data_size) {

    LSTATUS result = RegSetKeyValue(
        handle_, 
        sub_key.c_str(),
        name.c_str(),
        type,
        reinterpret_cast<const BYTE*>(data),
        data_size);

    ZAF_THROW_IF_SYSTEM_ERROR(result);
}


RegistryView RegistryKey::GetViewForSubKey(std::optional<RegistryView> expected_view) const {

    if (expected_view.has_value() && (view_ != RegistryView::Default)) {
        ZAF_EXPECT(*expected_view == view_);
    }

    return expected_view.value_or(view_);
}

}

