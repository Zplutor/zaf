#pragma once

#include <Windows.h>
#include <gtest/gtest.h>

class RegistryTest : public testing::Test {
public:
    void SetUp() override {
        RegDeleteKey(HKEY_CURRENT_USER, RegistryTestPath);
    }

    void TearDown() override {
        RegDeleteKey(HKEY_CURRENT_USER, RegistryTestPath);
    }

protected:
    static constexpr const wchar_t* RegistryTestPath = L"Software\\Zaf\\Unittest\\Registry";
};