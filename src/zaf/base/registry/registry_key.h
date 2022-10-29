#pragma once

#include <Windows.h>
#include <optional>
#include <string>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/base/registry/registry_rights.h>
#include <zaf/base/registry/registry_value.h>
#include <zaf/base/registry/registry_view.h>

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

    explicit operator bool() const {
        return !!handle_;
    }

    RegistryKey CreateSubKey(const std::wstring& sub_key) {
        return InnerCreateSubKey(
            sub_key,
            RegistryRights::Read | RegistryRights::Write,
            std::nullopt);
    }

    RegistryKey CreateSubKey(const std::wstring& sub_key, RegistryRights rights) {
        return InnerCreateSubKey(sub_key, rights, std::nullopt);
    }

    RegistryKey CreateSubKey(const std::wstring& sub_key, RegistryView view) {
        return InnerCreateSubKey(sub_key, RegistryRights::Read | RegistryRights::Write, view);
    }

    RegistryKey CreateSubKey(
        const std::wstring& sub_key,
        RegistryView view,
        RegistryRights rights) {

        return InnerCreateSubKey(sub_key, rights, view);
    }
    
    RegistryKey OpenSubKey(const std::wstring& sub_key) {
        return InnerOpenSubKey(sub_key, RegistryRights::Read, std::nullopt);
    }

    RegistryKey OpenSubKey(const std::wstring& sub_key, RegistryRights rights) {
        return InnerOpenSubKey(sub_key, rights, std::nullopt);
    }

    RegistryKey OpenSubKey(const std::wstring& sub_key, RegistryView view) {
        return InnerOpenSubKey(sub_key, RegistryRights::Read, view);
    }

    RegistryKey OpenSubKey(const std::wstring& sub_key, RegistryView view, RegistryRights rights) {
        return InnerOpenSubKey(sub_key, rights, view);
    }

    void DeleteSubKey(const std::wstring& sub_key) {
        InnerDeleteSubKey(sub_key, std::nullopt);
    }

    void DeleteSubKey(const std::wstring& sub_key, RegistryView view) {
        InnerDeleteSubKey(sub_key, view);
    }

    void DeleteValue(const std::wstring& name);

    RegistryValue GetValue(const std::wstring& sub_key, const std::wstring& name) const;
    RegistryValue GetValue(const std::wstring& name) const {
        return GetValue(std::wstring{}, name);
    }

    std::wstring GetStringValue(const std::wstring& sub_key, const std::wstring& name) const;
    std::wstring GetStringValue(const std::wstring& name) const {
        return GetStringValue(std::wstring{}, name);
    }

    std::wstring GetExpandableStringValue(
        const std::wstring& sub_key, 
        const std::wstring& name) const;
    std::wstring GetExpandableStringValue(const std::wstring& name) const {
        return GetExpandableStringValue(std::wstring{}, name);
    }

    std::vector<std::wstring> GetMultiStringValue(
        const std::wstring& sub_key, 
        const std::wstring& name) const;
    std::vector<std::wstring> GetMultiStringValue(const std::wstring& name) const {
        return GetMultiStringValue(std::wstring{}, name);
    }

    std::uint32_t GetDWordValue(const std::wstring& sub_key, const std::wstring& name) const;
    std::uint32_t GetDWordValue(const std::wstring& name) const {
        return GetDWordValue(std::wstring{}, name);
    }

    std::uint64_t GetQWordValue(const std::wstring& sub_key, const std::wstring& name) const;
    std::uint64_t GetQWordValue(const std::wstring& name) const {
        return GetQWordValue(std::wstring{}, name);
    }

    void SetStringValue(
        const std::wstring& sub_key, 
        const std::wstring& name,
        const std::wstring& value);
    void SetStringValue(const std::wstring& name, const std::wstring& value) {
        SetStringValue(std::wstring{}, name, value);
    }

    void SetExpandableStringValue(
        const std::wstring& sub_key, 
        const std::wstring& name, 
        const std::wstring& value);
    void SetExpandableStringValue(const std::wstring& name, const std::wstring& value) {
        SetExpandableStringValue(std::wstring{}, name, value);
    }

    void SetMultiStringValue(
        const std::wstring& sub_key,
        const std::wstring& name,
        const std::vector<std::wstring>& value);
    void SetMultiStringValue(const std::wstring& name, const std::vector<std::wstring>& value) {
        SetMultiStringValue(std::wstring{}, name, value);
    }

    void SetDWordValue(
        const std::wstring& sub_key, 
        const std::wstring& name, 
        std::uint32_t value);
    void SetDWordValue(const std::wstring& name, std::uint32_t value) {
        SetDWordValue(std::wstring{}, name, value);
    }

    void SetQWordValue(
        const std::wstring& sub_key,
        const std::wstring& name,
        std::uint64_t value);
    void SetQWordValue(const std::wstring& name, std::uint64_t value) {
        SetQWordValue(std::wstring{}, name, value);
    }

private:
    RegistryKey InnerCreateSubKey(
        const std::wstring& sub_key, 
        RegistryRights rights,
        std::optional<RegistryView> view);

    RegistryKey InnerOpenSubKey(
        const std::wstring& sub_key,
        RegistryRights rights,
        std::optional<RegistryView> view);

    void InnerDeleteSubKey(const std::wstring& sub_key, std::optional<RegistryView> view);

    RegistryValue InnerGetValue(
        const std::wstring& sub_key, 
        const std::wstring& name, 
        DWORD flags) const;

    void InnerSetValue(
        const std::wstring& sub_key,
        const std::wstring& name,
        DWORD type,
        const void* data,
        DWORD data_size);

    RegistryView GetViewForSubKey(std::optional<RegistryView> expected_view) const;

private:
    HKEY handle_{};
    RegistryView view_{ RegistryView::Default };
};

}