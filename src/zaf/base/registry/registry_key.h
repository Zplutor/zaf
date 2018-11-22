#pragma once

#include <Windows.h>
#include "zaf/base/error.h"
#include "zaf/base/registry/registry_rights.h"

namespace zaf {

class RegistryKey {
public:
    RegistryKey() {

    }

    explicit RegistryKey(HKEY handle) : handle_(handle) {        

    }

    ~RegistryKey();

    RegistryKey(RegistryKey&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    RegistryKey& operator=(RegistryKey&& other) {
        handle_ = other.handle_;
        other.handle_ = nullptr;
        return *this;
    }

    RegistryKey(const RegistryKey&) = delete;
    RegistryKey& operator=(const RegistryKey&) = delete;

    HKEY GetHandle() const {
        return handle_;
    }

    RegistryKey CreateSubKey(const std::wstring& subKey, std::error_code& error) {
        return CreateSubKey(subKey, RegistryRights::Read | RegistryRights::Write, error);
    }
    RegistryKey CreateSubKey(const std::wstring& subKey) {
        std::error_code error;
        auto result = CreateSubKey(subKey, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    RegistryKey CreateSubKey(const std::wstring& subKey, RegistryRights rights, std::error_code& error);
    RegistryKey CreateSubKey(const std::wstring& subKey, RegistryRights rights) {
        std::error_code error;
        auto result = OpenSubKey(subKey, rights, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    RegistryKey OpenSubKey(const std::wstring& subKey, std::error_code& error) {
        return OpenSubKey(subKey, RegistryRights::Read, error);
    }
    RegistryKey OpenSubKey(const std::wstring& subKey) {
        std::error_code error;
        auto result = OpenSubKey(subKey, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    RegistryKey OpenSubKey(const std::wstring& subKey, RegistryRights rights, std::error_code& error);
    RegistryKey OpenSubKey(const std::wstring& subKey, RegistryRights rights) {
        std::error_code error;
        auto result = OpenSubKey(subKey, rights, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    std::wstring GetDefaultValue(std::error_code& error) {
        return GetStringValue({}, error);
    }
    std::wstring GetDefaultValue() {
        std::error_code error;
        auto result = GetDefaultValue(error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    std::wstring GetStringValue(const std::wstring& name, std::error_code& error);
    std::wstring GetStringValue(const std::wstring& name) {
        std::error_code error;
        auto result = GetStringValue(name, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }
    
    std::uint32_t GetDWordValue(const std::wstring& name, std::error_code& error);
    std::uint32_t GetDWordValue(const std::wstring& name) {
        std::error_code error;
        auto result = GetDWordValue(name, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    std::uint64_t GetQWordValue(const std::wstring& name, std::error_code& error);
    std::uint64_t GetQWordValue(const std::wstring& name) {
        std::error_code error;
        auto result = GetQWordValue(name, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    void SetDefaultValue(const std::wstring& value, std::error_code& error) {
        SetStringValue({}, value, error);
    }
    void SetDefaultValue(const std::wstring& value) {
        std::error_code error;
        SetDefaultValue(value, error);
        ZAF_CHECK_ERROR(error);
    }

    void SetStringValue(const std::wstring& name, const std::wstring& value, std::error_code& error);
    void SetStringValue(const std::wstring& name, const std::wstring& value) {
        std::error_code error;
        SetStringValue(name, value, error);
        ZAF_CHECK_ERROR(error);
    }

    void SetDWordValue(const std::wstring& name, std::uint32_t value, std::error_code& error);
    void SetDWordValue(const std::wstring& name, std::uint32_t value) {
        std::error_code error;
        SetDWordValue(name, value, error);
        ZAF_CHECK_ERROR(error);
    }

    void SetQWordValue(const std::wstring& name, std::uint64_t value, std::error_code& error);
    void SetQWordValue(const std::wstring& name, std::uint64_t value) {
        std::error_code error;
        SetQWordValue(name, value, error);
        ZAF_CHECK_ERROR(error);
    }

private:
    std::vector<BYTE> GetValue(const std::wstring& name, DWORD expected_type, std::error_code& error);
    void SetValue(const std::wstring& name, DWORD type, const void* data, DWORD data_size, std::error_code& error);

private:
    HKEY handle_{};
};

}