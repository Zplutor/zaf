#include "registry_test.h"

zaf::RegistryRights RegistryTest::CheckRegistryKeyRights(const zaf::RegistryKey& key) {

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


void RegistryTest::CheckIfWOW64TestKeysExist(bool& key32_exists, bool& key64_exists) {

    HKEY key32{};
    RegOpenKeyEx(
        HKEY_CURRENT_USER,
        RegistryWOW64TestPath_32View,
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
        RegistryWOW64TestPath,
        0,
        KEY_READ,
        &key64);

    key64_exists = !!key64;
    if (key64) {
        RegCloseKey(key64);
    }
}