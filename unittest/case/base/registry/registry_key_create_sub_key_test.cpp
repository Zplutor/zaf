#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>
#include "registry_test.h"

namespace {

zaf::RegistryRights CheckRegistryKeyRights(const zaf::RegistryKey& key) {

    zaf::RegistryRights result{ zaf::RegistryRights::None };

    DWORD test_value = 447;
    LSTATUS error = RegSetValueEx(
        key.Handle(),
        L"DwordValue",
        0,
        REG_DWORD,
        reinterpret_cast<BYTE*>(&test_value),
        sizeof(test_value));

    if (error == ERROR_SUCCESS) {
        result |= zaf::RegistryRights::Write;
    }
    else if (error != ERROR_ACCESS_DENIED) {
        return zaf::RegistryRights::None;
    }

    DWORD value_length = sizeof(test_value);
    error = RegGetValue(
        key.Handle(),
        nullptr,
        L"DwordValue",
        RRF_RT_DWORD,
        nullptr,
        &test_value,
        &value_length);

    if (error == ERROR_SUCCESS || error == ERROR_FILE_NOT_FOUND) {
        result |= zaf::RegistryRights::Read;
    }
    else if (error != ERROR_ACCESS_DENIED) {
        return zaf::RegistryRights::None;
    }

    return result;
}


void CheckIfWOW64TestKeysExist(bool& key32_exists, bool& key64_exists) {

    HKEY key32{};
    RegOpenKeyEx(
        HKEY_CURRENT_USER,
        L"Software\\Classes\\WOW6432Node\\CLSID\\Zaf\\Unittest\\Registry",
        0,
        KEY_READ,
        &key32);

    key32_exists = !!key32;
    if (key32) {
        RegCloseKey(key32);
    }

    HKEY key64{};
    RegOpenKeyEx(
        HKEY_CURRENT_USER,
        L"Software\\Classes\\CLSID\\Zaf\\Unittest\\Registry",
        0,
        KEY_READ,
        &key64);

    key64_exists = !!key64;
    if (key64) {
        RegCloseKey(key64);
    }
}

}


TEST_F(RegistryTest, CreateSubKeyWithPathOnly) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(RegistryTestPath);
    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Default);

    //Check if the key has write and read rights.
    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read | zaf::RegistryRights::Write);
}


TEST_F(RegistryTest, CreateSubKeyWithRights) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath, 
        zaf::RegistryRights::Read);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Default);

    //Check if the key has read right only.
    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read);
}


TEST_F(RegistryTest, CreateSubKeyWithView) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry32);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Registry32);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read | zaf::RegistryRights::Write);
}


TEST_F(RegistryTest, CreateSubKeyWithViewAndRights) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry64,
        zaf::RegistryRights::Read);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Registry64);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read);
}


TEST_F(RegistryTest, CreateSubKeyWithDifferentView) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath, 
        zaf::RegistryView::Registry64);

    ASSERT_THROW(key.CreateSubKey(L"32View", zaf::RegistryView::Registry32), std::logic_error);
}


TEST_F(RegistryTest, CreateSubKey_32View) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry32);

    bool key32_exists{};
    bool key64_exists{};
    CheckIfWOW64TestKeysExist(key32_exists, key64_exists);
    ASSERT_TRUE(key32_exists);
    ASSERT_FALSE(key64_exists);
}


TEST_F(RegistryTest, CreateSubKey_64View) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry64);

    bool key32_exists{};
    bool key64_exists{};
    CheckIfWOW64TestKeysExist(key32_exists, key64_exists);
    ASSERT_FALSE(key32_exists);
    ASSERT_TRUE(key64_exists);
}