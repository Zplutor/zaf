#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>
#include "registry_test.h"

namespace {

class RegistrySetValueTest : public RegistryTest {

};

}

TEST_F(RegistrySetValueTest, SetDWordValue) {

    constexpr const wchar_t* DWordValueName = L"DWord";
    constexpr std::uint32_t DWordValue = 0x1232239d;

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    key.SetDWordValue(DWordValueName, DWordValue);

    DWORD data_type{};
    DWORD dword_data{};
    DWORD data_length{ sizeof(dword_data) };
    RegGetValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        DWordValueName,
        RRF_RT_DWORD,
        &data_type,
        &dword_data,
        &data_length);

    ASSERT_EQ(data_type, REG_DWORD);
    ASSERT_EQ(dword_data, DWordValue);
}


TEST_F(RegistrySetValueTest, SetDWordValueWithSubKey) {

    constexpr const wchar_t* DWordValueName = L"DWord";
    constexpr std::uint32_t DWordValue = 0x38dcb122;

    zaf::Registry::CurrentUser().SetDWordValue(RegistryTestPath, DWordValueName, DWordValue);

    auto dword = zaf::Registry::CurrentUser().GetDWordValue(RegistryTestPath, DWordValueName);
    ASSERT_EQ(dword, DWordValue);
}


TEST_F(RegistrySetValueTest, SetQWordValue) {

    constexpr const wchar_t* QWordValueName = L"QWord";
    constexpr std::uint64_t QWordValue = 0x230923f32381b444;

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    key.SetQWordValue(QWordValueName, QWordValue);

    DWORD data_type{};
    std::uint64_t qword_data{};
    DWORD data_length{ sizeof(qword_data) };
    RegGetValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        QWordValueName,
        RRF_RT_QWORD,
        &data_type,
        &qword_data,
        &data_length);

    ASSERT_EQ(data_type, REG_QWORD);
    ASSERT_EQ(qword_data, QWordValue);
}


TEST_F(RegistrySetValueTest, SetQWordValueWithSubKey) {

    constexpr const wchar_t* QWordValueName = L"QWord";
    constexpr std::uint64_t QWordValue = 0x3238823842929928;

    zaf::Registry::CurrentUser().SetQWordValue(RegistryTestPath, QWordValueName, QWordValue);

    auto qword = zaf::Registry::CurrentUser().GetQWordValue(RegistryTestPath, QWordValueName);
    ASSERT_EQ(qword, QWordValue);
}


TEST_F(RegistrySetValueTest, SetStringValue) {

    constexpr const wchar_t* StringValueName = L"StringValue";
    constexpr const wchar_t* StringValue = L"This is a string.";

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    key.SetStringValue(StringValueName, StringValue);

    DWORD data_type{};
    std::wstring data(100, L'\0');
    DWORD data_length = static_cast<DWORD>(data.size() * sizeof(wchar_t));
    RegGetValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        StringValueName,
        RRF_RT_REG_SZ,
        &data_type,
        &data[0],
        &data_length);

    ASSERT_EQ(data_type, REG_SZ);
    ASSERT_EQ(data_length, (std::wcslen(StringValue) + 1) * 2);
    ASSERT_EQ(std::wcscmp(data.data(), StringValue), 0);
}


TEST_F(RegistrySetValueTest, SetStringValueWithSubKey) {

    constexpr const wchar_t* StringValueName = L"StringValue";
    constexpr const wchar_t* StringValue = L"This is a string in sub key.";

    zaf::Registry::CurrentUser().SetStringValue(RegistryTestPath, StringValueName, StringValue);

    auto string = zaf::Registry::CurrentUser().GetStringValue(RegistryTestPath, StringValueName);
    ASSERT_EQ(string, StringValue);
}


TEST_F(RegistrySetValueTest, SetExpandableStringValue) {

    constexpr const wchar_t* StringValueName = L"ExpandableStringValue";
    constexpr const wchar_t* StringValue = L"This is %path%";

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    key.SetExpandableStringValue(StringValueName, StringValue);

    DWORD data_type{};
    std::wstring data(100, L'\0');
    DWORD data_length = static_cast<DWORD>(data.size() * sizeof(wchar_t));
    RegGetValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        StringValueName,
        RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
        &data_type,
        &data[0],
        &data_length);

    ASSERT_EQ(data_type, REG_EXPAND_SZ);
    ASSERT_EQ(data_length, (std::wcslen(StringValue) + 1) * 2);
    ASSERT_EQ(std::wcscmp(data.data(), StringValue), 0);
}


TEST_F(RegistrySetValueTest, SetExpandableStringValueWithSubKey) {

    constexpr const wchar_t* StringValueName = L"ExpandableStringValue";
    constexpr const wchar_t* StringValue = L"This is a %path%.";

    zaf::Registry::CurrentUser().SetExpandableStringValue(
        RegistryTestPath,
        StringValueName,
        StringValue);

    auto string = zaf::Registry::CurrentUser().GetExpandableStringValue(
        RegistryTestPath,
        StringValueName);

    ASSERT_EQ(string, StringValue);
}


TEST_F(RegistrySetValueTest, SetMultiStringValue) {

    constexpr const wchar_t* MultiStringValueName = L"MultiStringValue";

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    std::vector<std::wstring> multi_string{
        L"aaa",
        L"bb",
        L"c"
    };
    key.SetMultiStringValue(MultiStringValueName, multi_string);

    DWORD data_type{};
    std::wstring data(100, L'\0');
    DWORD data_length = static_cast<DWORD>(data.size() * sizeof(wchar_t));
    RegGetValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        MultiStringValueName,
        RRF_RT_REG_MULTI_SZ,
        &data_type,
        &data[0],
        &data_length);

    ASSERT_EQ(data_type, REG_MULTI_SZ);

    std::size_t expected_length{};
    for (const auto& each_string : multi_string) {
        expected_length += each_string.length() + 1;
    }
    expected_length += 1;

    ASSERT_EQ(data_length, expected_length * 2);
    ASSERT_EQ(std::memcmp(data.data(), L"aaa\0bb\0c\0\0", data_length), 0);
}


TEST_F(RegistrySetValueTest, SetMultiStringValueWithSubKey) {

    constexpr const wchar_t* MultiStringValueName = L"MultiStringValue";

    std::vector<std::wstring> multi_string{
        L"d",
        L"ee",
        L"fff"
    };

    zaf::Registry::CurrentUser().SetMultiStringValue(
        RegistryTestPath,
        MultiStringValueName,
        multi_string);

    auto actual = zaf::Registry::CurrentUser().GetMultiStringValue(
        RegistryTestPath,
        MultiStringValueName);

    ASSERT_EQ(actual, multi_string);
}