#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/registry/registry.h>

namespace {

class RegistryTest : public testing::Test {
public:
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
}