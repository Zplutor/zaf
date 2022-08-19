#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>

namespace {

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

}


TEST_F(RegistryTest, GetNumericValue) {

    constexpr const wchar_t* DWordValueName = L"DWord";
    constexpr std::uint32_t DWordValue = 0xaccdef78;
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        DWordValueName,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&DWordValue),
        sizeof(DWordValue));

    constexpr const wchar_t* QWordValueName = L"QWord";
    constexpr std::uint64_t QWordValue = 0xbef13834ac3ea2ee;
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        QWordValueName,
        REG_QWORD,
        reinterpret_cast<const BYTE*>(&QWordValue),
        sizeof(QWordValue));

    auto key = zaf::Registry::CurrentUser().OpenSubKey(RegistryTestPath);
    ASSERT_EQ(key.GetDWordValue(DWordValueName), DWordValue);
    ASSERT_EQ(key.GetQWordValue(QWordValueName), QWordValue);

    auto dword = zaf::Registry::CurrentUser().GetDWordValue(RegistryTestPath, DWordValueName);
    ASSERT_EQ(dword, DWordValue);
    auto qword = zaf::Registry::CurrentUser().GetQWordValue(RegistryTestPath, QWordValueName);
    ASSERT_EQ(qword, QWordValue);
}


TEST_F(RegistryTest, GetStringValue) {

    constexpr const wchar_t* NullTerminatedStringValueName = L"NullTerminatedString";
    constexpr const wchar_t* NullTerminatedStringValue = L"This is a null-terminated string.";
    RegSetKeyValue(
        HKEY_CURRENT_USER, 
        RegistryTestPath, 
        NullTerminatedStringValueName,
        REG_SZ, 
        NullTerminatedStringValue,
        // +1 for store terminated \0 into registry.
        static_cast<DWORD>(std::wcslen(NullTerminatedStringValue) + 1) * 2); 

    auto key = zaf::Registry::CurrentUser().OpenSubKey(RegistryTestPath);
    auto string = key.GetStringValue(NullTerminatedStringValueName);
    ASSERT_EQ(string.length(), std::wcslen(NullTerminatedStringValue));
    ASSERT_EQ(string, NullTerminatedStringValue);

    constexpr const wchar_t* NotNullTerminatedStringValueName = L"NotNullTerminatedString";
    constexpr const wchar_t* NotNullTerminatedStringValue = L"Theres is no terminated null.";
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        NotNullTerminatedStringValueName,
        REG_SZ,
        NotNullTerminatedStringValue,
        static_cast<DWORD>(std::wcslen(NotNullTerminatedStringValue)) * 2);

    string = key.GetStringValue(NotNullTerminatedStringValueName);
    ASSERT_EQ(string.length(), std::wcslen(NotNullTerminatedStringValue));
    ASSERT_EQ(string, NotNullTerminatedStringValue);

    constexpr const wchar_t* LongStringValueName = L"LongString";
    constexpr const wchar_t* LongStringValue =
        L"01234567890123456789012345678901234567890123456789"
        L"skhfkhoi32901ojlfljd;fe903e3oejop3k210393ijfknckdj"
        L"kj90022919903j93eok3mifj390339jf393ei93ei93ie30iof";
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        LongStringValueName,
        REG_SZ,
        LongStringValue,
        static_cast<DWORD>(std::wcslen(LongStringValue)) * 2);

    string = key.GetStringValue(LongStringValueName);
    ASSERT_EQ(string, LongStringValue);

    string = zaf::Registry::CurrentUser().GetStringValue(RegistryTestPath, LongStringValueName);
    ASSERT_EQ(string, LongStringValue);
}


TEST_F(RegistryTest, GetExpandableStringValue) {

    constexpr const wchar_t* ExpandableStringValueName = L"ExpandableStringValue";
    constexpr const wchar_t* ExpandableStringValue = L"This is an expandable %path%.";
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        ExpandableStringValueName,
        REG_EXPAND_SZ,
        ExpandableStringValue,
        static_cast<DWORD>(std::wcslen(ExpandableStringValue)) * 2);

    auto key = zaf::Registry::CurrentUser().OpenSubKey(RegistryTestPath);
    auto string = key.GetExpandableStringValue(ExpandableStringValueName);
    ASSERT_EQ(string, ExpandableStringValue);

    string = zaf::Registry::CurrentUser().GetExpandableStringValue(
        RegistryTestPath, 
        ExpandableStringValueName);
    ASSERT_EQ(string, ExpandableStringValue);
}


TEST_F(RegistryTest, GetMultiStringValue) {

    constexpr const wchar_t MultiStringValue[] = L"one\0two\0three\0\0";
    const std::vector<std::wstring> ExpectedMultiStrings{
        L"one",
        L"two",
        L"three"
    };

    constexpr const wchar_t* NullTerminatedStringValueName = L"NullTerminatedMultiString";
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        NullTerminatedStringValueName,
        REG_MULTI_SZ,
        MultiStringValue,
        static_cast<DWORD>(std::size(MultiStringValue)) * 2);

    auto key = zaf::Registry::CurrentUser().OpenSubKey(RegistryTestPath);
    auto multi_string = key.GetMultiStringValue(NullTerminatedStringValueName);
    ASSERT_EQ(multi_string, ExpectedMultiStrings);

    constexpr const wchar_t* NotNullTerminatedStringValueName = L"NotNullTerminatedMultiString";
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        NotNullTerminatedStringValueName,
        REG_MULTI_SZ,
        MultiStringValue,
        static_cast<DWORD>(std::size(MultiStringValue) - 1) * 2);

    multi_string = key.GetMultiStringValue(NotNullTerminatedStringValueName);
    ASSERT_EQ(multi_string, ExpectedMultiStrings);

    constexpr const wchar_t* EmptyStringValueName = L"EmptyString";
    RegSetKeyValue(
        HKEY_CURRENT_USER,
        RegistryTestPath,
        EmptyStringValueName,
        REG_MULTI_SZ,
        L"",
        2); //Write one \0 only.

    multi_string = key.GetMultiStringValue(EmptyStringValueName);
    ASSERT_TRUE(multi_string.empty());

    multi_string = zaf::Registry::CurrentUser().GetMultiStringValue(
        RegistryTestPath, 
        NullTerminatedStringValueName);
    ASSERT_EQ(multi_string, ExpectedMultiStrings);
}


TEST_F(RegistryTest, SetDWordValue) {

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


TEST_F(RegistryTest, SetDWordValueWithSubKey) {

    constexpr const wchar_t* DWordValueName = L"DWord";
    constexpr std::uint32_t DWordValue = 0x38dcb122;

    zaf::Registry::CurrentUser().SetDWordValue(RegistryTestPath, DWordValueName, DWordValue);

    auto dword = zaf::Registry::CurrentUser().GetDWordValue(RegistryTestPath, DWordValueName);
    ASSERT_EQ(dword, DWordValue);
}


TEST_F(RegistryTest, SetQWordValue) {

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


TEST_F(RegistryTest, SetQWordValueWithSubKey) {

    constexpr const wchar_t* QWordValueName = L"QWord";
    constexpr std::uint64_t QWordValue = 0x3238823842929928;

    zaf::Registry::CurrentUser().SetQWordValue(RegistryTestPath, QWordValueName, QWordValue);

    auto qword = zaf::Registry::CurrentUser().GetQWordValue(RegistryTestPath, QWordValueName);
    ASSERT_EQ(qword, QWordValue);
}


TEST_F(RegistryTest, SetStringValue) {

    constexpr const wchar_t* StringValueName = L"StringValue";
    constexpr const wchar_t* StringValue = L"This is a string.";

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath, 
        zaf::RegistryRights::Write);

    key.SetStringValue(StringValueName, StringValue);

    DWORD data_type{};
    std::wstring data(100, L'\0');
    DWORD data_length = data.size() * sizeof(wchar_t);
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


TEST_F(RegistryTest, SetStringValueWithSubKey) {

    constexpr const wchar_t* StringValueName = L"StringValue";
    constexpr const wchar_t* StringValue = L"This is a string in sub key.";

    zaf::Registry::CurrentUser().SetStringValue(RegistryTestPath, StringValueName, StringValue);

    auto string = zaf::Registry::CurrentUser().GetStringValue(RegistryTestPath, StringValueName);
    ASSERT_EQ(string, StringValue);
}


TEST_F(RegistryTest, SetExpandableStringValue) {

    constexpr const wchar_t* StringValueName = L"ExpandableStringValue";
    constexpr const wchar_t* StringValue = L"This is %path%";

    auto key = zaf::Registry::CurrentUser().CreateSubKey(
        RegistryTestPath,
        zaf::RegistryRights::Write);

    key.SetExpandableStringValue(StringValueName, StringValue);

    DWORD data_type{};
    std::wstring data(100, L'\0');
    DWORD data_length = data.size() * sizeof(wchar_t);
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


TEST_F(RegistryTest, SetExpandableStringValueWithSubKey) {

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


TEST_F(RegistryTest, SetMultiStringValue) {

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
    DWORD data_length = data.size() * sizeof(wchar_t);
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


TEST_F(RegistryTest, SetMultiStringValueWithSubKey) {

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