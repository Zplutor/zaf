#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>
#include "registry_test.h"

namespace {

class RegistryOpenSubKeyTest : public RegistryTest {
public:
    void SetUp() override {

        __super::SetUp();

        CreateTestKey(RegistryTestPath);
        CreateTestKey(RegistryWOW64TestPath);
        CreateTestKey(std::wstring(RegistryWOW64TestPath) + L"\\" + View64SubKey);
        CreateTestKey(RegistryWOW64TestPath_32View);
        CreateTestKey(std::wstring(RegistryWOW64TestPath_32View) + L"\\" + View32SubKey);
    }

protected:
    static constexpr const wchar_t* View64SubKey = L"64";
    static constexpr const wchar_t* View32SubKey = L"32";

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

TEST_F(RegistryOpenSubKeyTest, OpenSubKeyWithPathOnly) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(RegistryTestPath);
    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Default);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read);
}


TEST_F(RegistryOpenSubKeyTest, OpenSubKeyWithRights) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Default);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Write);
}


TEST_F(RegistryOpenSubKeyTest, OpenSubKeyWithView) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry32);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Registry32);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read);
}


TEST_F(RegistryOpenSubKeyTest, OpenSubKeyWithViewAndRights) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry64,
        zaf::RegistryRights::Write);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Registry64);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Write);
}


TEST_F(RegistryOpenSubKeyTest, OpenSubKeyWithDifferentView) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry32);

    ASSERT_THROW(key.OpenSubKey(L"64View", zaf::RegistryView::Registry64), std::logic_error);
}


TEST_F(RegistryOpenSubKeyTest, OpenSubKey_32View) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryWOW64TestPath, 
        zaf::RegistryView::Registry32);

    ASSERT_EQ(key.View(), zaf::RegistryView::Registry32);

    auto sub_key1 = key.OpenSubKey(View32SubKey);
    ASSERT_EQ(sub_key1.View(), zaf::RegistryView::Registry32);

    auto sub_key2 = key.OpenSubKey(View32SubKey, zaf::RegistryView::Registry32);
    ASSERT_EQ(sub_key2.View(), zaf::RegistryView::Registry32);
}


TEST_F(RegistryOpenSubKeyTest, OpenSubKey_64View) {

    auto key = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry64);

    ASSERT_EQ(key.View(), zaf::RegistryView::Registry64);

    auto sub_key1 = key.OpenSubKey(View64SubKey);
    ASSERT_EQ(sub_key1.View(), zaf::RegistryView::Registry64);

    auto sub_key2 = key.OpenSubKey(View64SubKey, zaf::RegistryView::Registry64);
    ASSERT_EQ(sub_key2.View(), zaf::RegistryView::Registry64);
}