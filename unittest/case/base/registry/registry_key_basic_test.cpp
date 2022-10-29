#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>

TEST(RegistryBasicTest, Construct) {

    {
        zaf::RegistryKey key;
        ASSERT_EQ(key.Handle(), nullptr);
        ASSERT_EQ(key.View(), zaf::RegistryView::Default);
    }

    {
        zaf::RegistryKey key(HKEY_CLASSES_ROOT);
        ASSERT_EQ(key.Handle(), HKEY_CLASSES_ROOT);
        ASSERT_EQ(key.View(), zaf::RegistryView::Default);
    }

    {
        zaf::RegistryKey key(HKEY_CURRENT_USER, zaf::RegistryView::Registry32);
        ASSERT_EQ(key.Handle(), HKEY_CURRENT_USER);
        ASSERT_EQ(key.View(), zaf::RegistryView::Registry32);
    }
}


TEST(RegistryBasicTest, Move) {

    zaf::RegistryKey key1(HKEY_LOCAL_MACHINE, zaf::RegistryView::Registry64);

    zaf::RegistryKey key2(std::move(key1));
    ASSERT_EQ(key2.Handle(), HKEY_LOCAL_MACHINE);
    ASSERT_EQ(key2.View(), zaf::RegistryView::Registry64);
    ASSERT_EQ(key1.Handle(), nullptr);
    ASSERT_EQ(key1.View(), zaf::RegistryView::Default);

    zaf::RegistryKey key3;
    key3 = std::move(key2);
    ASSERT_EQ(key3.Handle(), HKEY_LOCAL_MACHINE);
    ASSERT_EQ(key3.View(), zaf::RegistryView::Registry64);
    ASSERT_EQ(key2.Handle(), nullptr);
    ASSERT_EQ(key2.View(), zaf::RegistryView::Default);
}


TEST(RegistryBasicTest, BoolTest) {

    zaf::RegistryKey key;
    if (key) {
        GTEST_FAIL();
    }

    if (!key) {

    }
    else {
        GTEST_FAIL();
    }

    key = zaf::Registry::CurrentUser();
    if (key) {

    }
    else {
        GTEST_FAIL();
    }

    if (!key) {
        GTEST_FAIL();
    }
}
