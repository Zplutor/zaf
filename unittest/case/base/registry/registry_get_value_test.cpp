#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>
#include "registry_test.h"

namespace {

class RegistryGetValueTest : public RegistryTest {
protected:
    static void WriteWOW64TestValues() {

        //Write to 64 view.
        auto key64 = zaf::Registry::CurrentUser().CreateSubKey(RegistryWOW64TestPath);
        key64.SetDWordValue(WOW64DWord, WOW64DWordValue64);
        key64.SetQWordValue(WOW64QWord, WOW64QWordValue64);
        key64.SetStringValue(WOW64String, WOW64StringValue64);
        key64.SetExpandableStringValue(WOW64ExpandableString, WOW64ExpandableStringValue64);
        key64.SetMultiStringValue(WOW64MultiString, WOW64MultiStringValue64());

        //Write to 32 view.
        auto key32 = zaf::Registry::CurrentUser().CreateSubKey(RegistryWOW64TestPath_32View);
        key32.SetDWordValue(WOW64DWord, WOW64DWordValue32);
        key32.SetQWordValue(WOW64QWord, WOW64QWordValue32);
        key32.SetStringValue(WOW64String, WOW64StringValue32);
        key32.SetExpandableStringValue(WOW64ExpandableString, WOW64ExpandableStringValue32);
        key32.SetMultiStringValue(WOW64MultiString, WOW64MultiStringValue32());
    }

protected:
    static constexpr const wchar_t* WOW64DWord = L"DWord";
    static constexpr std::uint32_t WOW64DWordValue64 = 64;
    static constexpr std::uint32_t WOW64DWordValue32 = 32;

    static constexpr const wchar_t* WOW64QWord = L"QWord";
    static constexpr std::uint64_t WOW64QWordValue64 = 6464;
    static constexpr std::uint64_t WOW64QWordValue32 = 3232;

    static constexpr const wchar_t* WOW64String = L"String";
    static constexpr const wchar_t* WOW64StringValue64 = L"64";
    static constexpr const wchar_t* WOW64StringValue32 = L"32";

    static constexpr const wchar_t* WOW64ExpandableString = L"ExpandableString";
    static constexpr const wchar_t* WOW64ExpandableStringValue64 = L"e64";
    static constexpr const wchar_t* WOW64ExpandableStringValue32 = L"e32";

    static constexpr const wchar_t* WOW64MultiString = L"MultiString";
    static std::vector<std::wstring> WOW64MultiStringValue64() {
        return { L"6", L"4" };
    }
    static std::vector<std::wstring> WOW64MultiStringValue32() {
        return { L"3", L"2" };
    }
};

}

TEST_F(RegistryGetValueTest, GetNumericValue) {

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


TEST_F(RegistryGetValueTest, GetStringValue) {

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


TEST_F(RegistryGetValueTest, GetExpandableStringValue) {

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


TEST_F(RegistryGetValueTest, GetMultiStringValue) {

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


TEST_F(RegistryGetValueTest, GetValueFrom32View) {

    WriteWOW64TestValues();

    auto root_key = zaf::Registry::CurrentUser();

    auto dword = root_key.GetDWordValue(
        RegistryWOW64TestPath, 
        WOW64DWord, 
        zaf::RegistryView::Registry32);
    ASSERT_EQ(dword, WOW64DWordValue32);

    auto qword = root_key.GetQWordValue(
        RegistryWOW64TestPath, 
        WOW64QWord, 
        zaf::RegistryView::Registry32);
    ASSERT_EQ(qword, WOW64QWordValue32);

    auto string = root_key.GetStringValue(
        RegistryWOW64TestPath, 
        WOW64String, 
        zaf::RegistryView::Registry32);
    ASSERT_EQ(string, WOW64StringValue32);

    auto expandable_string = root_key.GetExpandableStringValue(
        RegistryWOW64TestPath,
        WOW64ExpandableString,
        zaf::RegistryView::Registry32);
    ASSERT_EQ(expandable_string, WOW64ExpandableStringValue32);

    auto multi_string = root_key.GetMultiStringValue(
        RegistryWOW64TestPath, 
        WOW64MultiString, 
        zaf::RegistryView::Registry32);
    ASSERT_EQ(multi_string, WOW64MultiStringValue32());

    auto key32 = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry32);
    ASSERT_EQ(key32.GetDWordValue(WOW64DWord), WOW64DWordValue32);
    ASSERT_EQ(key32.GetQWordValue(WOW64QWord), WOW64QWordValue32);
    ASSERT_EQ(key32.GetStringValue(WOW64String), WOW64StringValue32);
    ASSERT_EQ(key32.GetExpandableStringValue(WOW64ExpandableString), WOW64ExpandableStringValue32);
    ASSERT_EQ(key32.GetMultiStringValue(WOW64MultiString), WOW64MultiStringValue32());
}


TEST_F(RegistryGetValueTest, GetValueFrom64View) {

    WriteWOW64TestValues();

    auto root_key = zaf::Registry::CurrentUser();

    auto dword = root_key.GetDWordValue(
        RegistryWOW64TestPath,
        WOW64DWord,
        zaf::RegistryView::Registry64);
    ASSERT_EQ(dword, WOW64DWordValue64);

    auto qword = root_key.GetQWordValue(
        RegistryWOW64TestPath,
        WOW64QWord,
        zaf::RegistryView::Registry64);
    ASSERT_EQ(qword, WOW64QWordValue64);

    auto string = root_key.GetStringValue(
        RegistryWOW64TestPath,
        WOW64String,
        zaf::RegistryView::Registry64);
    ASSERT_EQ(string, WOW64StringValue64);

    auto expandable_string = root_key.GetExpandableStringValue(
        RegistryWOW64TestPath,
        WOW64ExpandableString,
        zaf::RegistryView::Registry64);
    ASSERT_EQ(expandable_string, WOW64ExpandableStringValue64);

    auto multi_string = root_key.GetMultiStringValue(
        RegistryWOW64TestPath,
        WOW64MultiString,
        zaf::RegistryView::Registry64);
    ASSERT_EQ(multi_string, WOW64MultiStringValue64());

    auto key64 = zaf::Registry::CurrentUser().OpenSubKey(
        RegistryWOW64TestPath,
        zaf::RegistryView::Registry64);
    ASSERT_EQ(key64.GetDWordValue(WOW64DWord), WOW64DWordValue64);
    ASSERT_EQ(key64.GetQWordValue(WOW64QWord), WOW64QWordValue64);
    ASSERT_EQ(key64.GetStringValue(WOW64String), WOW64StringValue64);
    ASSERT_EQ(key64.GetExpandableStringValue(WOW64ExpandableString), WOW64ExpandableStringValue64);
    ASSERT_EQ(key64.GetMultiStringValue(WOW64MultiString), WOW64MultiStringValue64());
}


TEST_F(RegistryGetValueTest, GetValueWithDifferentView) {

    WriteWOW64TestValues();

    std::wstring key_path = RegistryWOW64TestPath;
    auto last_delimiter = key_path.find_last_of(L'\\');

    auto sub_key_name = key_path.substr(last_delimiter + 1);
    key_path.resize(last_delimiter);

    auto key = zaf::Registry::CurrentUser().OpenSubKey(key_path, zaf::RegistryView::Registry64);

    ASSERT_THROW(
        key.GetStringValue(sub_key_name, WOW64String, zaf::RegistryView::Registry32), 
        std::logic_error);
}