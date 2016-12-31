#include <gtest/gtest.h>
#include <zaf/base/string/to_numeric.h>

#define TEST_TO_NUMERIC_SUCCESS(type, cstr, base, value)  \
{                                                         \
type v;                                                   \
bool r;                                                   \
zaf::ToNumericOptions options;                            \
options.Base(base);                                       \
r = zaf::TryToNumeric(cstr, v, options);                  \
ASSERT_EQ(true, r);                                       \
ASSERT_EQ(value, v);                                      \
r = zaf::TryToNumeric(L##cstr, v, options);               \
ASSERT_EQ(true, r);                                       \
ASSERT_EQ(value, v);                                      \
}


#define TEST_TO_INTEGER_NUMERIC_SUCCESS(type, value, dec, hex, oct, bin) \
TEST_TO_NUMERIC_SUCCESS(type, dec, 10, value)                            \
TEST_TO_NUMERIC_SUCCESS(type, hex, 16, value)                            \
TEST_TO_NUMERIC_SUCCESS(type, oct, 8, value)                             \
TEST_TO_NUMERIC_SUCCESS(type, bin, 2, value)


#define TEST_TO_FLOAT_NUMERIC_SUCCESS(type, value, cstr) \
TEST_TO_NUMERIC_SUCCESS(type, cstr, 10, value)


#define TEST_TO_FLOAT_OVERLOAD(type)       \
{                                          \
type v;                                    \
bool r;                                    \
std::string string("10");                  \
std::wstring wstring(L"10");               \
r = zaf::TryToNumeric(string, v);          \
ASSERT_TRUE(r);                            \
ASSERT_EQ(v, 10);                          \
r = zaf::TryToNumeric(wstring, v);         \
ASSERT_TRUE(r);                            \
ASSERT_EQ(v, 10);                          \
v = zaf::ToNumeric<type>(string);          \
ASSERT_EQ(v, 10);                          \
v = zaf::ToNumeric<type>(wstring);         \
ASSERT_EQ(v, 10);                          \
v = zaf::ToNumeric<type>(string.c_str());  \
ASSERT_EQ(v, 10);                          \
v = zaf::ToNumeric<type>(wstring.c_str()); \
ASSERT_EQ(v, 10);                          \
}


#define TEST_TO_NUMERIC_FAILURE(type, cstr, base)  \
{                                                  \
type v = 8;                                        \
bool r;                                            \
zaf::ToNumericOptions options;                     \
options.Base(base);                                \
r = zaf::TryToNumeric(cstr, v, options);           \
ASSERT_FALSE(r);                                   \
ASSERT_EQ(v, 8);                                   \
r = zaf::TryToNumeric(L##cstr, v, options);        \
ASSERT_FALSE(r);                                   \
ASSERT_EQ(v, 8);                                   \
}


#define TEST_TO_NUMERIC_FAILURE_ALL_TYPES(cstr)       \
TEST_TO_NUMERIC_FAILURE(char, cstr, 10)               \
TEST_TO_NUMERIC_FAILURE(signed char, cstr, 10)        \
TEST_TO_NUMERIC_FAILURE(unsigned char, cstr, 10)      \
TEST_TO_NUMERIC_FAILURE(wchar_t, cstr, 10)            \
TEST_TO_NUMERIC_FAILURE(short, cstr, 10)              \
TEST_TO_NUMERIC_FAILURE(unsigned short, cstr, 10)     \
TEST_TO_NUMERIC_FAILURE(int, cstr, 10)                \
TEST_TO_NUMERIC_FAILURE(unsigned int, cstr, 10)       \
TEST_TO_NUMERIC_FAILURE(long, cstr, 10)               \
TEST_TO_NUMERIC_FAILURE(unsigned long, cstr, 10)      \
TEST_TO_NUMERIC_FAILURE(long long, cstr, 10)          \
TEST_TO_NUMERIC_FAILURE(unsigned long long, cstr, 10) \
TEST_TO_NUMERIC_FAILURE(float, cstr, 10)              \
TEST_TO_NUMERIC_FAILURE(double, cstr, 10)             \
TEST_TO_NUMERIC_FAILURE(long double, cstr, 10)        


#define TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES(cstr, base) \
TEST_TO_NUMERIC_FAILURE(char, cstr, base)                 \
TEST_TO_NUMERIC_FAILURE(signed char, cstr, base)          \
TEST_TO_NUMERIC_FAILURE(unsigned char, cstr, base)        \
TEST_TO_NUMERIC_FAILURE(wchar_t, cstr, base)              \
TEST_TO_NUMERIC_FAILURE(short, cstr, base)                \
TEST_TO_NUMERIC_FAILURE(unsigned short, cstr, base)       \
TEST_TO_NUMERIC_FAILURE(int, cstr, base)                  \
TEST_TO_NUMERIC_FAILURE(unsigned int, cstr, base)         \
TEST_TO_NUMERIC_FAILURE(long, cstr, base)                 \
TEST_TO_NUMERIC_FAILURE(unsigned long, cstr, base)        \
TEST_TO_NUMERIC_FAILURE(long long, cstr, base)            \
TEST_TO_NUMERIC_FAILURE(unsigned long long, cstr, base)   


#define TEST_TO_NUMERIC_FAILURE_FLOAT_TYPES(cstr) \
TEST_TO_NUMERIC_FAILURE(float, cstr, 10)          \
TEST_TO_NUMERIC_FAILURE(double, cstr, 10)         \
TEST_TO_NUMERIC_FAILURE(long double, cstr, 10)    \


TEST(ToNumeric, IntegerSuccess) {

	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, -128, "-128", "80", "200", "10000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, -127, "-127", "81", "201", "10000001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, -100, "-100", "9c", "234", "10011100");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, -1,   "-1",   "ff", "377", "11111111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, 0,    "0",    "0",  "0",   "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, 1,    "1",    "1",  "1",   "1");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, '0',  "48",   "30", "60",  "110000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, 100,  "100",  "64", "144", "1100100");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(char, 127,  "127",  "7f", "177", "1111111");

	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, -128, "-128", "80", "200", "10000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, -127, "-127", "81", "201", "10000001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, -100, "-100", "9c", "234", "10011100");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, -1,   "-1",   "ff", "377", "11111111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, 0,    "0",    "0",  "0",   "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, 1,    "1",    "1",  "1",   "1");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, '0',  "48",   "30", "60",  "110000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, 100,  "100",  "64", "144", "1100100");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(signed char, 127,  "127",  "7f", "177", "1111111");

	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned char, 0,   "0",   "0",  "0",   "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned char, 128, "128", "80", "200", "10000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned char, 131, "131", "83", "203", "10000011");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned char, 255, "255", "ff", "377", "11111111");
	
	TEST_TO_INTEGER_NUMERIC_SUCCESS(wchar_t, 0, "0", "0", "0", "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(wchar_t, L'Œ“', "25105", "6211", "61021", "110001000010001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(wchar_t, 65535, "65535", "ffff", "177777", "1111111111111111");

	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, -32768, "-32768", "8000", "100000", "1000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, -32767, "-32767", "8001", "100001", "1000000000000001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, -30000, "-30000", "8ad0", "105320", "1000101011010000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, -230,   "-230",   "ff1a", "177432", "1111111100011010");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, -1,     "-1",     "ffff", "177777", "1111111111111111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, 0,      "0",      "0",    "0",      "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, 1,      "1",      "1",    "1",      "1");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, 30000,  "30000",  "7530", "72460",  "111010100110000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(short, 32767,  "32767",  "7fff", "77777",  "111111111111111");

	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned short, 0,     "0",     "0",    "0",      "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned short, 330,   "330",   "14a",  "512",    "101001010");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned short, 32768, "32768", "8000", "100000", "1000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned short, 65535, "65535", "ffff", "177777", "1111111111111111");
	 
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, -2147483647 - 1, "-2147483648", "0x80000000", "020000000000", "10000000000000000000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, -2147483647,     "-2147483647", "0X80000001", "020000000001", "10000000000000000000000000000001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, -12345678,       "-12345678",   "0xff439eb2", "037720717262", "11111111010000111001111010110010");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, -1,              "-1",          "0Xffffffff", "037777777777", "11111111111111111111111111111111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, 0,               "0",           "0x0",        "00",           "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, 1,               "1",           "0X1",        "01",           "1");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, 12345678,        "12345678",    "0xbc614e",   "057060516",    "101111000110000101001110");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(int, 2147483647,      "2147483647",  "0X7fffffff", "017777777777", "1111111111111111111111111111111"); 

	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned int, 0,          "0",          "0",        "0",           "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned int, 12345678,   "12345678",   "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned int, 2147483648, "2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned int, 4294967295, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");

	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, -2147483647 - 1, "-2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, -2147483647,     "-2147483647", "80000001", "20000000001", "10000000000000000000000000000001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, -87654321,       "-87654321",   "FAC6804F", "37261500117", "11111010110001101000000001001111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, -12345678,       "-12345678",   "FF439EB2", "37720717262", "11111111010000111001111010110010");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, -1,              "-1",          "FFfFfffF", "37777777777", "11111111111111111111111111111111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, 0,               "0",           "0",        "0",           "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, 1,               "1",           "1",        "1",           "1");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, 12345678,        "12345678",    "bC614e",   "57060516",    "101111000110000101001110");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, 87654321,        "87654321",    "5397Fb1",  "516277661",   "101001110010111111110110001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long, 2147483647,      "2147483647",  "7ffFfFff", "17777777777", "1111111111111111111111111111111"); 

	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long, 0,          "0",          "0",        "0",           "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long, 12345678,   "12345678",   "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long, 87654321,   "87654321",   "5397fb1",  "516277661",   "101001110010111111110110001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long, 2147483648, "2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long, 4294967295, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");

	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, -9223372036854775807 - 1, "-9223372036854775808", "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, -9223372036854775807,     "-9223372036854775807", "8000000000000001", "1000000000000000000001", "1000000000000000000000000000000000000000000000000000000000000001");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, -123456789123456789,      "-123456789123456789",  "fe4964b4532fa0eb", "1771113113212313720353", "1111111001001001011001001011010001010011001011111010000011101011");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, -1,                       "-1",                   "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, 0,                        "0",                    "0",                "0",                      "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, 1,                        "1",                    "1",                "1",                      "1");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, 123456789123456789,       "123456789123456789",   "1b69b4bacd05f15",  "6664664565464057425",    "110110110100110110100101110101100110100000101111100010101");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(long long, 9223372036854775807,      "9223372036854775807",  "7fffffffffffffff", "777777777777777777777",  "111111111111111111111111111111111111111111111111111111111111111"); 
	
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long long, 0,                    "0",                    "0",                "0",                      "0");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long long, 123456789123456789,   "123456789123456789",   "1b69b4bacd05f15",  "6664664565464057425",    "110110110100110110100101110101100110100000101111100010101");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long long, 9223372036854775808,  "9223372036854775808",  "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TEST_TO_INTEGER_NUMERIC_SUCCESS(unsigned long long, 18446744073709551615, "18446744073709551615", "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
}


TEST(ToNumeric, FloatSuccess) {

    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, -3.4028234e38f, "-3.4028234e38");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, -78.f, "-78");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, -3.14159f, "-3.14159");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, -1.17549e-38f, "-1.17549e-38");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, 0.f, "0");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, 0.f, "0.0");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, 1.17549e-38f, "1.17549e-38");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, 3.14159f, "3.14159");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, 78.f, "78");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(float, 3.4028234e38f, "3.4028234e38");

    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, -1.7976931348623157e308, "-1.7976931348623157e308");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, -78., "-78");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, -3.14159, "-3.14159");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, -2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, 0., "0");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, 0., "0.0");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, 2.2250738585072014e-308, "2.2250738585072014e-308");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, 3.14159, "3.14159");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, 78., "78");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(double, 1.7976931348623157e308, "1.7976931348623157e308");

    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, -1.7976931348623157e308, "-1.7976931348623157e308");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, -78., "-78");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, -3.14159, "-3.14159");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, -2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, 0., "0");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, 0., "0.0");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, 2.2250738585072014e-308, "2.2250738585072014e-308");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, 3.14159, "3.14159");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, 78., "78");
    TEST_TO_FLOAT_NUMERIC_SUCCESS(long double, 1.7976931348623157e308, "1.7976931348623157e308");
}


/**
 This test aims to make sure all overload functions are able to compile on all supported types,
 doesn't pay must attention on the accuracy of conversions.
 */
TEST(ToNumeric, Overload) {

    TEST_TO_FLOAT_OVERLOAD(char);
    TEST_TO_FLOAT_OVERLOAD(signed char);
    TEST_TO_FLOAT_OVERLOAD(unsigned char);
    TEST_TO_FLOAT_OVERLOAD(wchar_t);
    TEST_TO_FLOAT_OVERLOAD(short);
    TEST_TO_FLOAT_OVERLOAD(unsigned short);
    TEST_TO_FLOAT_OVERLOAD(int);
    TEST_TO_FLOAT_OVERLOAD(unsigned int);
    TEST_TO_FLOAT_OVERLOAD(long);
    TEST_TO_FLOAT_OVERLOAD(unsigned long);
    TEST_TO_FLOAT_OVERLOAD(long long);
    TEST_TO_FLOAT_OVERLOAD(unsigned long long);
    TEST_TO_FLOAT_OVERLOAD(float);
    TEST_TO_FLOAT_OVERLOAD(double);
    TEST_TO_FLOAT_OVERLOAD(long double);
}


TEST(ToNumeric, CommomFailure) {

    //Incomplete string
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES(" 10");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("\n10");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("\t10");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("\b10");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("10\n");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("10\t");
    TEST_TO_NUMERIC_FAILURE_ALL_TYPES("10\b");

    //Larger value to smaller type
    TEST_TO_NUMERIC_FAILURE(char, "128", 10);
    TEST_TO_NUMERIC_FAILURE(char, "-129", 10);
    TEST_TO_NUMERIC_FAILURE(signed char, "128", 10);
    TEST_TO_NUMERIC_FAILURE(signed char, "-129", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned char, "256", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned short, "65536", 10);
    TEST_TO_NUMERIC_FAILURE(short, "32768", 10);
    TEST_TO_NUMERIC_FAILURE(short, "-32769", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned short, "65536", 10);
    TEST_TO_NUMERIC_FAILURE(int, "2147483648", 10);
    TEST_TO_NUMERIC_FAILURE(int, "-2147483649", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned int, "4294967296", 10);
    TEST_TO_NUMERIC_FAILURE(long, "2147483648", 10);
    TEST_TO_NUMERIC_FAILURE(long, "-2147483649", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned long, "4294967296", 10);
    TEST_TO_NUMERIC_FAILURE(long long, "9223372036854775808", 10);
    TEST_TO_NUMERIC_FAILURE(long long, "-9223372036854775809", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned long long, "18446744073709551616", 10);

    //The string to floating point conversion functions in std return an 
    //incorrect error when converted value is out of range on VisualStudio2013 
    //due to a bug. So we disable these limit value tests for now.
    /*
    TEST_TO_NUMERIC_FAILURE(float, "-1.7976931348623157e308", 10);
    TEST_TO_NUMERIC_FAILURE(float, "-2.2250738585072014e-308", 10);
    TEST_TO_NUMERIC_FAILURE(float, "2.2250738585072014e-308", 10);
    TEST_TO_NUMERIC_FAILURE(float, "1.7976931348623157e308", 10);

    TEST_TO_NUMERIC_FAILURE(double, "-1.7976931348623157e309", 10);
    TEST_TO_NUMERIC_FAILURE(double, "-2.2250738585072014e-309", 10);
    TEST_TO_NUMERIC_FAILURE(double, "2.2250738585072014e-309", 10);
    TEST_TO_NUMERIC_FAILURE(double, "1.7976931348623157e309", 10);

    TEST_TO_NUMERIC_FAILURE(long double, "-1.7976931348623157e309", 10);
    TEST_TO_NUMERIC_FAILURE(long double, "-2.2250738585072014e-309", 10);
    TEST_TO_NUMERIC_FAILURE(long double, "2.2250738585072014e-309", 10);
    TEST_TO_NUMERIC_FAILURE(long double, "1.7976931348623157e309", 10);
    */
}


TEST(ToNumeric, IntegerFailure) {

    //Non-decimal with sign char
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("-1", 2);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("+1", 2);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("-7", 8);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("+8", 8);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("-f", 16);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("+f", 16);

    //Invalid chars
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("121", 2);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("787", 8);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("9a9", 10);
    TEST_TO_NUMERIC_FAILURE_INTEGER_TYPES("fgf", 16);

    //Signed value to unsigned type
    TEST_TO_NUMERIC_FAILURE(unsigned char, "-9", 10);
    TEST_TO_NUMERIC_FAILURE(wchar_t, "-9", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned short, "-9", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned int, "-9", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned long, "-9", 10);
    TEST_TO_NUMERIC_FAILURE(unsigned long long, "-9", 10);
}


TEST(ToNumeric, FloatFailure) {

    //Non-decimal
    TEST_TO_NUMERIC_FAILURE(float, "10", 16);
    TEST_TO_NUMERIC_FAILURE(double, "10", 8);
    TEST_TO_NUMERIC_FAILURE(long double, "10", 2);

    //Invalid chars
    TEST_TO_NUMERIC_FAILURE_FLOAT_TYPES("abcd");
    TEST_TO_NUMERIC_FAILURE_FLOAT_TYPES("3.1f5");
    TEST_TO_NUMERIC_FAILURE_FLOAT_TYPES("3.1e1d");
}