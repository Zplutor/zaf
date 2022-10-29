#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>
#include "registry_test.h"

namespace {

class RegistryDeleteSubKeyTest : public RegistryTest {
public:
    void SetUp() override {

        __super::SetUp();

        CreateTestKey(RegistryTestPath);
        CreateTestKey(RegistryWOW64TestPath);
        CreateTestKey(RegistryWOW64TestPath_32View);
    }

protected:
    static bool IsSubKeyExistent(const std::wstring& path) {

        HKEY key{};
        RegOpenKeyEx(
            HKEY_CURRENT_USER,
            path.c_str(),
            0,
            KEY_READ,
            &key);

        bool is_existent = !!key;
        RegCloseKey(key);
        return is_existent;
    }

private:
    void CreateTestKey(const std::wstring& path) {

        HKEY key{};
        RegCreateKeyEx(
            HKEY_CURRENT_USER,
            path.c_str(),
            0,
            nullptr,
            0,
            KEY_READ | KEY_WRITE,
            nullptr,
            &key,
            nullptr);

        RegCloseKey(key);
    }
};

}


TEST_F(RegistryDeleteSubKeyTest, DeleteSubKeyWithPathOnly) {

    zaf::Registry::CurrentUser().DeleteSubKey(RegistryTestPath);

    ASSERT_FALSE(IsSubKeyExistent(RegistryTestPath));
}


TEST_F(RegistryDeleteSubKeyTest, DeleteSubKey_32View) {

    zaf::Registry::CurrentUser().DeleteSubKey(RegistryWOW64TestPath, zaf::RegistryView::Registry32);

    bool key32_exists{};
    bool key64_exists{};
    CheckIfWOW64TestKeysExist(key32_exists, key64_exists);
    ASSERT_FALSE(key32_exists);
    ASSERT_TRUE(key64_exists);
}


TEST_F(RegistryDeleteSubKeyTest, DeleteSubKey_64View) {

    zaf::Registry::CurrentUser().DeleteSubKey(RegistryWOW64TestPath, zaf::RegistryView::Registry64);

    bool key32_exists{};
    bool key64_exists{};
    CheckIfWOW64TestKeysExist(key32_exists, key64_exists);
    ASSERT_TRUE(key32_exists);
    ASSERT_FALSE(key64_exists);
}


TEST_F(RegistryDeleteSubKeyTest, DeleteSubKeyWithDifferentView) {

    std::wstring key_path = RegistryTestPath;
    auto last_delimiter = key_path.find_last_of(L'\\');

    auto sub_key_name = key_path.substr(last_delimiter + 1);
    key_path.resize(last_delimiter);

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        key_path,
        zaf::RegistryView::Registry32,
        zaf::RegistryRights::Write);

    ASSERT_THROW(key.DeleteSubKey(sub_key_name, zaf::RegistryView::Registry64), std::logic_error);
}