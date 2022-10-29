#pragma once

#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry_key.h>

class RegistryTest : public testing::Test {
public:
    void SetUp() override {
        DeleteKeys();
    }

    void TearDown() override {
        DeleteKeys();
    }

protected:
    static constexpr const wchar_t* RegistryTestPath = L"Software\\Zaf\\Unittest\\Registry";

    static constexpr const wchar_t* RegistryWOW64TestPath = 
        L"Software\\Classes\\CLSID\\Zaf\\Unittest\\Registry";

    static constexpr const wchar_t* RegistryWOW64TestPath_32View =
        L"Software\\Classes\\WOW6432Node\\CLSID\\Zaf\\Unittest\\Registry";

    static zaf::RegistryRights CheckRegistryKeyRights(const zaf::RegistryKey& key);
    static void CheckIfWOW64TestKeysExist(bool& key32_exists, bool& key64_exists);

private:
    void DeleteKeys() {

        RegDeleteTree(HKEY_CURRENT_USER, RegistryTestPath);

        RegDeleteTree(HKEY_CURRENT_USER, RegistryWOW64TestPath);
        RegDeleteTree(HKEY_CURRENT_USER, RegistryWOW64TestPath_32View);
    }
};