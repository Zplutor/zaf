#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>
#include "registry_test.h"

namespace {

class RegistryCreateSubKeyTest : public RegistryTest {

};

}


TEST_F(RegistryCreateSubKeyTest, CreateSubKeyWithPathOnly) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(RegistryTestPath);
    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Default);

    //Check if the key has write and read rights.
    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read | zaf::RegistryRights::Write);
}


TEST_F(RegistryCreateSubKeyTest, CreateSubKeyWithRights) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath, 
        zaf::RegistryRights::Read);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Default);

    //Check if the key has read right only.
    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read);
}


TEST_F(RegistryCreateSubKeyTest, CreateSubKeyWithView) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry32);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Registry32);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read | zaf::RegistryRights::Write);
}


TEST_F(RegistryCreateSubKeyTest, CreateSubKeyWithViewAndRights) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryView::Registry64,
        zaf::RegistryRights::Read);

    ASSERT_NE(key.Handle(), nullptr);
    ASSERT_EQ(key.View(), zaf::RegistryView::Registry64);

    ASSERT_EQ(CheckRegistryKeyRights(key), zaf::RegistryRights::Read);
}


TEST_F(RegistryCreateSubKeyTest, CreateSubKeyWithDifferentView) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath, 
        zaf::RegistryView::Registry64);

    ASSERT_THROW(key.CreateSubKey(L"32View", zaf::RegistryView::Registry32), std::logic_error);
}


TEST_F(RegistryCreateSubKeyTest, CreateSubKey_32View) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry32);

    bool key32_exists{};
    bool key64_exists{};
    CheckIfWOW64TestKeysExist(key32_exists, key64_exists);
    ASSERT_TRUE(key32_exists);
    ASSERT_FALSE(key64_exists);

    auto sub_key_default_view = key.CreateSubKey(L"SubKey_Default");
    ASSERT_EQ(sub_key_default_view.View(), zaf::RegistryView::Registry32);

    auto sub_key_32_view = key.CreateSubKey(L"SubKey_32View", zaf::RegistryView::Registry32);
    ASSERT_EQ(sub_key_32_view.View(), zaf::RegistryView::Registry32);
}


TEST_F(RegistryCreateSubKeyTest, CreateSubKey_64View) {

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry64);

    bool key32_exists{};
    bool key64_exists{};
    CheckIfWOW64TestKeysExist(key32_exists, key64_exists);
    ASSERT_FALSE(key32_exists);
    ASSERT_TRUE(key64_exists);

    auto sub_key_default_view = key.CreateSubKey(L"SubKey_Default");
    ASSERT_EQ(sub_key_default_view.View(), zaf::RegistryView::Registry64);

    auto sub_key_64_view = key.CreateSubKey(L"SubKey_64View", zaf::RegistryView::Registry64);
    ASSERT_EQ(sub_key_64_view.View(), zaf::RegistryView::Registry64);
}