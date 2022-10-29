#pragma once

#include <Windows.h>
#include <gtest/gtest.h>

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

private:
    void DeleteKeys() {

        RegDeleteTree(HKEY_CURRENT_USER, RegistryTestPath);

        RegDeleteKeyEx(HKEY_CURRENT_USER, RegistryWOW64TestPath, KEY_WOW64_32KEY, 0);
        RegDeleteKeyEx(HKEY_CURRENT_USER, RegistryWOW64TestPath, KEY_WOW64_64KEY, 0);
    }
};