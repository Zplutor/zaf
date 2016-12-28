#include <gtest/gtest.h>
#include <zaf/base/string/to_numeric.h>

#define TEST_TONUMERIC_SUCCESS(type, cstr, base, value)  \
{                                                        \
std::string str(cstr);                                   \
std::wstring wstr(L##cstr);                              \
type v;                                                  \
bool r;                                                  \
zaf::ToNumericOptions options;                           \
options.Base(base);                                      \
v = zaf::ToNumeric<type>(cstr, options);                 \
ASSERT_EQ(value, v);                                     \
v = zaf::ToNumeric<type>(L##cstr, options);              \
ASSERT_EQ(value, v);                                     \
v = zaf::ToNumeric<type>(str, options);                  \
ASSERT_EQ(value, v);                                     \
v = zaf::ToNumeric<type>(wstr, options);                 \
ASSERT_EQ(value, v);                                     \
r = zaf::TryToNumeric(cstr, v, options);                 \
ASSERT_EQ(true, r);                                      \
ASSERT_EQ(value, v);                                     \
r = zaf::TryToNumeric(L##cstr, v, options);              \
ASSERT_EQ(true, r);                                      \
ASSERT_EQ(value, v);                                     \
r = zaf::TryToNumeric(str, v, options);                  \
ASSERT_EQ(true, r);                                      \
ASSERT_EQ(value, v);                                     \
r = zaf::TryToNumeric(wstr, v, options);                 \
ASSERT_EQ(true, r);                                      \
ASSERT_EQ(value, v);                                     \
}

#define TEST_TONUMERIC_SUCCESS_ALL(type, value, dec, hex, oct, bin) \
TEST_TONUMERIC_SUCCESS(type, dec, 10, value)                        \
TEST_TONUMERIC_SUCCESS(type, hex, 16, value)                        \
TEST_TONUMERIC_SUCCESS(type, oct, 8, value)                         \
TEST_TONUMERIC_SUCCESS(type, bin, 2, value)

#define TEST_TONUMERIC_FAILURE(type, cstr, base)  \
{                                                 \
std::string str(cstr);                            \
std::wstring wstr(L##cstr);                       \
bool r;                                           \
zaf::ToNumericOptions options;                    \
options.Base(base);                               \
type v = 1;                                       \
v = zaf::ToNumeric<type>(cstr, options);          \
ASSERT_EQ(0, v);                                  \
v = 1;                                            \
v = zaf::ToNumeric<type>(L##cstr, options);       \
ASSERT_EQ(0, v);                                  \
v = 1;                                            \
v = zaf::ToNumeric<type>(str, options);           \
ASSERT_EQ(0, v);                                  \
v = 1;                                            \
v = zaf::ToNumeric<type>(wstr, options);          \
ASSERT_EQ(0, v);                                  \
v = 1;                                            \
r = zaf::TryToNumeric(cstr, v, options);          \
ASSERT_EQ(false, r);                              \
ASSERT_EQ(1, v);                                  \
r = zaf::TryToNumeric(L##cstr, v, options);       \
ASSERT_EQ(false, r);                              \
ASSERT_EQ(1, v);                                  \
r = zaf::TryToNumeric(str, v, options);           \
ASSERT_EQ(false, r);                              \
ASSERT_EQ(1, v);                                  \
r = zaf::TryToNumeric(wstr, v, options);          \
ASSERT_EQ(false, r);                              \
ASSERT_EQ(1, v);                                  \
}

#define TEST_TONUMERIC_FAILURE_ALL_TYPES(cstr, base)  \
TEST_TONUMERIC_FAILURE(char, cstr, base)              \
TEST_TONUMERIC_FAILURE(signed char, cstr, base)       \
TEST_TONUMERIC_FAILURE(unsigned char, cstr, base)     \
TEST_TONUMERIC_FAILURE(wchar_t, cstr, base)           \
TEST_TONUMERIC_FAILURE(short, cstr, base)             \
TEST_TONUMERIC_FAILURE(unsigned short, cstr, base)    \
TEST_TONUMERIC_FAILURE(int, cstr, base)               \
TEST_TONUMERIC_FAILURE(unsigned int, cstr, base)      \
TEST_TONUMERIC_FAILURE(long, cstr, base)              \
TEST_TONUMERIC_FAILURE(unsigned long, cstr, base)     \
TEST_TONUMERIC_FAILURE(long long, cstr, base)         \
TEST_TONUMERIC_FAILURE(unsigned long long, cstr, base)

#define TEST_TONUMERIC_FAILURE_ALL_BASES(type, dec, hex, oct, bin)   \
TEST_TONUMERIC_FAILURE(type, dec, 10)                                \
TEST_TONUMERIC_FAILURE(type, hex, 16)                                \
TEST_TONUMERIC_FAILURE(type, oct, 8)                                 \
TEST_TONUMERIC_FAILURE(type, bin, 2)

#define TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES(cstr)   \
TEST_TONUMERIC_FAILURE_ALL_TYPES(cstr, 10)                 \
TEST_TONUMERIC_FAILURE_ALL_TYPES(cstr, 16)                 \
TEST_TONUMERIC_FAILURE_ALL_TYPES(cstr, 8)                  \
TEST_TONUMERIC_FAILURE_ALL_TYPES(cstr, 2)                  \

#define TEST_TONUMERIC_FAILURE_ALL(dec, hex, oct, bin) \
TEST_TONUMERIC_FAILURE_ALL_TYPES(dec, 10)              \
TEST_TONUMERIC_FAILURE_ALL_TYPES(hex, 16)              \
TEST_TONUMERIC_FAILURE_ALL_TYPES(oct, 8)               \
TEST_TONUMERIC_FAILURE_ALL_TYPES(bin, 2)               \

TEST(ToNumeric, IntegerSuccess) {

	TEST_TONUMERIC_SUCCESS_ALL(char, -128, "-128", "80", "200", "10000000");
	TEST_TONUMERIC_SUCCESS_ALL(char, -127, "-127", "81", "201", "10000001");
	TEST_TONUMERIC_SUCCESS_ALL(char, -100, "-100", "9c", "234", "10011100");
	TEST_TONUMERIC_SUCCESS_ALL(char, -1,   "-1",   "ff", "377", "11111111");
	TEST_TONUMERIC_SUCCESS_ALL(char, 0,    "0",    "0",  "0",   "0");
	TEST_TONUMERIC_SUCCESS_ALL(char, 1,    "1",    "1",  "1",   "1");
	TEST_TONUMERIC_SUCCESS_ALL(char, '0',  "48",   "30", "60",  "110000");
	TEST_TONUMERIC_SUCCESS_ALL(char, 100,  "100",  "64", "144", "1100100");
	TEST_TONUMERIC_SUCCESS_ALL(char, 127,  "127",  "7f", "177", "1111111");

	TEST_TONUMERIC_SUCCESS_ALL(signed char, -128, "-128", "80", "200", "10000000");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, -127, "-127", "81", "201", "10000001");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, -100, "-100", "9c", "234", "10011100");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, -1,   "-1",   "ff", "377", "11111111");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, 0,    "0",    "0",  "0",   "0");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, 1,    "1",    "1",  "1",   "1");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, '0',  "48",   "30", "60",  "110000");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, 100,  "100",  "64", "144", "1100100");
	TEST_TONUMERIC_SUCCESS_ALL(signed char, 127,  "127",  "7f", "177", "1111111");

	TEST_TONUMERIC_SUCCESS_ALL(unsigned char, 0,   "0",   "0",  "0",   "0");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned char, 128, "128", "80", "200", "10000000");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned char, 131, "131", "83", "203", "10000011");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned char, 255, "255", "ff", "377", "11111111");
	
	TEST_TONUMERIC_SUCCESS_ALL(wchar_t, 0, "0", "0", "0", "0");
	TEST_TONUMERIC_SUCCESS_ALL(wchar_t, L'Œ“', "25105", "6211", "61021", "110001000010001");
	TEST_TONUMERIC_SUCCESS_ALL(wchar_t, 65535, "65535", "ffff", "177777", "1111111111111111");

	TEST_TONUMERIC_SUCCESS_ALL(short, -32768, "-32768", "8000", "100000", "1000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(short, -32767, "-32767", "8001", "100001", "1000000000000001");
	TEST_TONUMERIC_SUCCESS_ALL(short, -30000, "-30000", "8ad0", "105320", "1000101011010000");
	TEST_TONUMERIC_SUCCESS_ALL(short, -230,   "-230",   "ff1a", "177432", "1111111100011010");
	TEST_TONUMERIC_SUCCESS_ALL(short, -1,     "-1",     "ffff", "177777", "1111111111111111");
	TEST_TONUMERIC_SUCCESS_ALL(short, 0,      "0",      "0",    "0",      "0");
	TEST_TONUMERIC_SUCCESS_ALL(short, 1,      "1",      "1",    "1",      "1");
	TEST_TONUMERIC_SUCCESS_ALL(short, 30000,  "30000",  "7530", "72460",  "111010100110000");
	TEST_TONUMERIC_SUCCESS_ALL(short, 32767,  "32767",  "7fff", "77777",  "111111111111111");

	TEST_TONUMERIC_SUCCESS_ALL(unsigned short, 0,     "0",     "0",    "0",      "0");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned short, 330,   "330",   "14a",  "512",    "101001010");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned short, 32768, "32768", "8000", "100000", "1000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned short, 65535, "65535", "ffff", "177777", "1111111111111111");
	 
	TEST_TONUMERIC_SUCCESS_ALL(int, -2147483647 - 1, "-2147483648", "0x80000000", "020000000000", "10000000000000000000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(int, -2147483647,     "-2147483647", "0X80000001", "020000000001", "10000000000000000000000000000001");
	TEST_TONUMERIC_SUCCESS_ALL(int, -12345678,       "-12345678",   "0xff439eb2", "037720717262", "11111111010000111001111010110010");
	TEST_TONUMERIC_SUCCESS_ALL(int, -1,              "-1",          "0Xffffffff", "037777777777", "11111111111111111111111111111111");
	TEST_TONUMERIC_SUCCESS_ALL(int, 0,               "0",           "0x0",        "00",           "0");
	TEST_TONUMERIC_SUCCESS_ALL(int, 1,               "1",           "0X1",        "01",           "1");
	TEST_TONUMERIC_SUCCESS_ALL(int, 12345678,        "12345678",    "0xbc614e",   "057060516",    "101111000110000101001110");
	TEST_TONUMERIC_SUCCESS_ALL(int, 2147483647,      "2147483647",  "0X7fffffff", "017777777777", "1111111111111111111111111111111"); 

	TEST_TONUMERIC_SUCCESS_ALL(unsigned int, 0,          "0",          "0",        "0",           "0");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned int, 12345678,   "12345678",   "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned int, 2147483648, "2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned int, 4294967295, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");

	TEST_TONUMERIC_SUCCESS_ALL(long, -2147483647 - 1, "-2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(long, -2147483647,     "-2147483647", "80000001", "20000000001", "10000000000000000000000000000001");
	TEST_TONUMERIC_SUCCESS_ALL(long, -87654321,       "-87654321",   "FAC6804F", "37261500117", "11111010110001101000000001001111");
	TEST_TONUMERIC_SUCCESS_ALL(long, -12345678,       "-12345678",   "FF439EB2", "37720717262", "11111111010000111001111010110010");
	TEST_TONUMERIC_SUCCESS_ALL(long, -1,              "-1",          "FFfFfffF", "37777777777", "11111111111111111111111111111111");
	TEST_TONUMERIC_SUCCESS_ALL(long, 0,               "0",           "0",        "0",           "0");
	TEST_TONUMERIC_SUCCESS_ALL(long, 1,               "1",           "1",        "1",           "1");
	TEST_TONUMERIC_SUCCESS_ALL(long, 12345678,        "12345678",    "bC614e",   "57060516",    "101111000110000101001110");
	TEST_TONUMERIC_SUCCESS_ALL(long, 87654321,        "87654321",    "5397Fb1",  "516277661",   "101001110010111111110110001");
	TEST_TONUMERIC_SUCCESS_ALL(long, 2147483647,      "2147483647",  "7ffFfFff", "17777777777", "1111111111111111111111111111111"); 

	TEST_TONUMERIC_SUCCESS_ALL(unsigned long, 0,          "0",          "0",        "0",           "0");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long, 12345678,   "12345678",   "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long, 87654321,   "87654321",   "5397fb1",  "516277661",   "101001110010111111110110001");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long, 2147483648, "2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long, 4294967295, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");

	TEST_TONUMERIC_SUCCESS_ALL(long long, -9223372036854775807 - 1, "-9223372036854775808", "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(long long, -9223372036854775807,     "-9223372036854775807", "8000000000000001", "1000000000000000000001", "1000000000000000000000000000000000000000000000000000000000000001");
	TEST_TONUMERIC_SUCCESS_ALL(long long, -123456789123456789,      "-123456789123456789",  "fe4964b4532fa0eb", "1771113113212313720353", "1111111001001001011001001011010001010011001011111010000011101011");
	TEST_TONUMERIC_SUCCESS_ALL(long long, -1,                       "-1",                   "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
	TEST_TONUMERIC_SUCCESS_ALL(long long, 0,                        "0",                    "0",                "0",                      "0");
	TEST_TONUMERIC_SUCCESS_ALL(long long, 1,                        "1",                    "1",                "1",                      "1");
	TEST_TONUMERIC_SUCCESS_ALL(long long, 123456789123456789,       "123456789123456789",   "1b69b4bacd05f15",  "6664664565464057425",    "110110110100110110100101110101100110100000101111100010101");
	TEST_TONUMERIC_SUCCESS_ALL(long long, 9223372036854775807,      "9223372036854775807",  "7fffffffffffffff", "777777777777777777777",  "111111111111111111111111111111111111111111111111111111111111111"); 
	
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long long, 0,                    "0",                    "0",                "0",                      "0");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long long, 123456789123456789,   "123456789123456789",   "1b69b4bacd05f15",  "6664664565464057425",    "110110110100110110100101110101100110100000101111100010101");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long long, 9223372036854775808,  "9223372036854775808",  "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TEST_TONUMERIC_SUCCESS_ALL(unsigned long long, 18446744073709551615, "18446744073709551615", "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
}


TEST(ToNumeric, IntegerFailure) {

	//Incomplete string
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES(" 10");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("\n10");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("\t10");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("\b10");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("10\n");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("10\t");
    TEST_TONUMERIC_FAILURE_ALL_TYPES_AND_BASES("10\b");

	//Non-decimal with sign
	TEST_TONUMERIC_FAILURE_ALL("", "-f", "-7", "-1");
	TEST_TONUMERIC_FAILURE_ALL("", "+f", "+7", "+1");

	//Invalid chars
	TEST_TONUMERIC_FAILURE_ALL("9a9", "fgf", "787", "121");

	//signed value to unsigned type
	TEST_TONUMERIC_FAILURE(unsigned char,      "-9", 10);
	TEST_TONUMERIC_FAILURE(wchar_t,            "-9", 10);
	TEST_TONUMERIC_FAILURE(unsigned short,     "-9", 10);
	TEST_TONUMERIC_FAILURE(unsigned int,       "-9", 10);
	TEST_TONUMERIC_FAILURE(unsigned long,      "-9", 10);
	TEST_TONUMERIC_FAILURE(unsigned long long, "-9", 10);
	
	//large value to small type
	TEST_TONUMERIC_FAILURE_ALL_BASES(char,          "128",  "100", "400", "100000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(char,          "-129", "100", "400", "100000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(signed char,   "128",  "100", "400", "100000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(signed char,   "-129", "100", "400", "100000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(unsigned char, "256",  "100", "400", "100000000");

	TEST_TONUMERIC_FAILURE_ALL_BASES(unsigned short, "65536",  "10000", "200000", "10000000000000000");

	TEST_TONUMERIC_FAILURE_ALL_BASES(short,          "32768",  "10000", "200000", "10000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(short,          "-32769", "10000", "200000", "10000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(unsigned short, "65536",  "10000", "200000", "10000000000000000");

	TEST_TONUMERIC_FAILURE_ALL_BASES(int,          "2147483648",  "100000000", "40000000000", "100000000000000000000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(int,          "-2147483649", "100000000", "40000000000", "100000000000000000000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(unsigned int, "4294967296",  "100000000", "40000000000", "100000000000000000000000000000000");

	TEST_TONUMERIC_FAILURE_ALL_BASES(long,          "2147483648",  "100000000", "40000000000", "100000000000000000000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(long,          "-2147483649", "100000000", "40000000000", "100000000000000000000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(unsigned long, "4294967296",  "100000000", "40000000000", "100000000000000000000000000000000");

	TEST_TONUMERIC_FAILURE_ALL_BASES(long long,          "9223372036854775808",  "10000000000000000", "2000000000000000000000", "10000000000000000000000000000000000000000000000000000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(long long,          "-9223372036854775809", "10000000000000000", "2000000000000000000000", "10000000000000000000000000000000000000000000000000000000000000000");
	TEST_TONUMERIC_FAILURE_ALL_BASES(unsigned long long, "18446744073709551616", "10000000000000000", "2000000000000000000000", "10000000000000000000000000000000000000000000000000000000000000000");
}
