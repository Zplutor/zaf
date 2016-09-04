#include <gtest/gtest.h>
#include <zaf/base/string/to_string.h>

#define TEST_TOSTRING(type, radix, value, expected)  \
{                                                    \
std::string str;                                     \
std::wstring wstr;                                   \
type v = value;                                      \
str = zaf::ToString(v, radix);                       \
ASSERT_EQ(expected, str);                            \
wstr = zaf::ToWideString(v, radix);                  \
ASSERT_EQ(L##expected, wstr);                        \
}                                             

#define TEST_TOSTRING_ALL(type, value, dec, hex, oct, bin)  \
TEST_TOSTRING(type, 10, value, dec)                         \
TEST_TOSTRING(type, 2, value, bin)                          \
TEST_TOSTRING(type, 8, value, oct)                          \
TEST_TOSTRING(type, 16, value, hex)                         

TEST(ToString, Integer) {

    TEST_TOSTRING_ALL(char, -128, "-128", "80", "200", "10000000");
	TEST_TOSTRING_ALL(char, -127, "-127", "81", "201", "10000001");
	TEST_TOSTRING_ALL(char, -100, "-100", "9c", "234", "10011100");
	TEST_TOSTRING_ALL(char, -1,   "-1",   "ff", "377", "11111111");
	TEST_TOSTRING_ALL(char, 0,    "0",    "0",  "0",   "0");
	TEST_TOSTRING_ALL(char, 1,    "1",    "1",  "1",   "1");
	TEST_TOSTRING_ALL(char, '0',  "48",   "30", "60",  "110000");
	TEST_TOSTRING_ALL(char, 100,  "100",  "64", "144", "1100100");
	TEST_TOSTRING_ALL(char, 127,  "127",  "7f", "177", "1111111");

	TEST_TOSTRING_ALL(signed char, -128, "-128", "80", "200", "10000000");
	TEST_TOSTRING_ALL(signed char, -127, "-127", "81", "201", "10000001");
	TEST_TOSTRING_ALL(signed char, -100, "-100", "9c", "234", "10011100");
	TEST_TOSTRING_ALL(signed char, -1,   "-1",   "ff", "377", "11111111");
	TEST_TOSTRING_ALL(signed char, 0,    "0",    "0",  "0",   "0");
	TEST_TOSTRING_ALL(signed char, 1,    "1",    "1",  "1",   "1");
	TEST_TOSTRING_ALL(signed char, '0',  "48",   "30", "60",  "110000");
	TEST_TOSTRING_ALL(signed char, 100,  "100",  "64", "144", "1100100");
	TEST_TOSTRING_ALL(signed char, 127,  "127",  "7f", "177", "1111111");

	TEST_TOSTRING_ALL(unsigned char, 0,   "0",   "0",  "0",   "0");
	TEST_TOSTRING_ALL(unsigned char, 128, "128", "80", "200", "10000000");
	TEST_TOSTRING_ALL(unsigned char, 131, "131", "83", "203", "10000011");
	TEST_TOSTRING_ALL(unsigned char, 255, "255", "ff", "377", "11111111");
	
	TEST_TOSTRING_ALL(wchar_t, 0, "0", "0", "0", "0");
	TEST_TOSTRING_ALL(wchar_t, L'Œ“', "25105", "6211", "61021", "110001000010001");
	TEST_TOSTRING_ALL(wchar_t, 65535, "65535", "ffff", "177777", "1111111111111111");

	TEST_TOSTRING_ALL(short, -32768, "-32768", "8000", "100000", "1000000000000000");
	TEST_TOSTRING_ALL(short, -32767, "-32767", "8001", "100001", "1000000000000001");
	TEST_TOSTRING_ALL(short, -30000, "-30000", "8ad0", "105320", "1000101011010000");
	TEST_TOSTRING_ALL(short, -230,   "-230",   "ff1a", "177432", "1111111100011010");
	TEST_TOSTRING_ALL(short, -1,     "-1",     "ffff", "177777", "1111111111111111");
	TEST_TOSTRING_ALL(short, 0,      "0",      "0",    "0",      "0");
	TEST_TOSTRING_ALL(short, 1,      "1",      "1",    "1",      "1");
	TEST_TOSTRING_ALL(short, 30000,  "30000",  "7530", "72460",  "111010100110000");
	TEST_TOSTRING_ALL(short, 32767,  "32767",  "7fff", "77777",  "111111111111111");

	TEST_TOSTRING_ALL(unsigned short, 0,     "0",     "0",    "0",      "0");
	TEST_TOSTRING_ALL(unsigned short, 330,   "330",   "14a",  "512",    "101001010");
	TEST_TOSTRING_ALL(unsigned short, 32768, "32768", "8000", "100000", "1000000000000000");
	TEST_TOSTRING_ALL(unsigned short, 65535, "65535", "ffff", "177777", "1111111111111111");
	 
	TEST_TOSTRING_ALL(int, -2147483647 - 1, "-2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TOSTRING_ALL(int, -2147483647,     "-2147483647", "80000001", "20000000001", "10000000000000000000000000000001");
	TEST_TOSTRING_ALL(int, -12345678,       "-12345678",   "ff439eb2", "37720717262", "11111111010000111001111010110010");
	TEST_TOSTRING_ALL(int, -1,              "-1",          "ffffffff", "37777777777", "11111111111111111111111111111111");
	TEST_TOSTRING_ALL(int, 0,               "0",           "0",        "0",           "0");
	TEST_TOSTRING_ALL(int, 1,               "1",           "1",        "1",           "1");
	TEST_TOSTRING_ALL(int, 12345678,        "12345678",    "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TOSTRING_ALL(int, 2147483647,      "2147483647",  "7fffffff", "17777777777", "1111111111111111111111111111111"); 

	TEST_TOSTRING_ALL(unsigned int, 0,          "0",          "0",        "0",           "0");
	TEST_TOSTRING_ALL(unsigned int, 12345678,   "12345678",   "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TOSTRING_ALL(unsigned int, 2147483648, "2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TOSTRING_ALL(unsigned int, 4294967295, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");

	TEST_TOSTRING_ALL(long, -2147483647 - 1, "-2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TOSTRING_ALL(long, -2147483647,     "-2147483647", "80000001", "20000000001", "10000000000000000000000000000001");
	TEST_TOSTRING_ALL(long, -87654321,       "-87654321",   "fac6804f", "37261500117", "11111010110001101000000001001111");
	TEST_TOSTRING_ALL(long, -12345678,       "-12345678",   "ff439eb2", "37720717262", "11111111010000111001111010110010");
	TEST_TOSTRING_ALL(long, -1,              "-1",          "ffffffff", "37777777777", "11111111111111111111111111111111");
	TEST_TOSTRING_ALL(long, 0,               "0",           "0",        "0",           "0");
	TEST_TOSTRING_ALL(long, 1,               "1",           "1",        "1",           "1");
	TEST_TOSTRING_ALL(long, 12345678,        "12345678",    "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TOSTRING_ALL(long, 87654321,        "87654321",    "5397fb1",  "516277661",   "101001110010111111110110001");
	TEST_TOSTRING_ALL(long, 2147483647,      "2147483647",  "7fffffff", "17777777777", "1111111111111111111111111111111"); 

	TEST_TOSTRING_ALL(unsigned long, 0,          "0",          "0",        "0",           "0");
	TEST_TOSTRING_ALL(unsigned long, 12345678,   "12345678",   "bc614e",   "57060516",    "101111000110000101001110");
	TEST_TOSTRING_ALL(unsigned long, 87654321,   "87654321",   "5397fb1",  "516277661",   "101001110010111111110110001");
	TEST_TOSTRING_ALL(unsigned long, 2147483648, "2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TEST_TOSTRING_ALL(unsigned long, 4294967295, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");

	TEST_TOSTRING_ALL(long long, -9223372036854775807 - 1, "-9223372036854775808", "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TEST_TOSTRING_ALL(long long, -9223372036854775807,     "-9223372036854775807", "8000000000000001", "1000000000000000000001", "1000000000000000000000000000000000000000000000000000000000000001");
	TEST_TOSTRING_ALL(long long, -123456789123456789,      "-123456789123456789",  "fe4964b4532fa0eb", "1771113113212313720353", "1111111001001001011001001011010001010011001011111010000011101011");
	TEST_TOSTRING_ALL(long long, -1,                       "-1",                   "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
	TEST_TOSTRING_ALL(long long, 0,                        "0",                    "0",                "0",                      "0");
	TEST_TOSTRING_ALL(long long, 1,                        "1",                    "1",                "1",                      "1");
	TEST_TOSTRING_ALL(long long, 123456789123456789,       "123456789123456789",   "1b69b4bacd05f15",  "6664664565464057425",    "110110110100110110100101110101100110100000101111100010101");
	TEST_TOSTRING_ALL(long long, 9223372036854775807,      "9223372036854775807",  "7fffffffffffffff", "777777777777777777777",  "111111111111111111111111111111111111111111111111111111111111111"); 
	
	TEST_TOSTRING_ALL(unsigned long long, 0,                    "0",                    "0",                "0",                      "0");
	TEST_TOSTRING_ALL(unsigned long long, 123456789123456789,   "123456789123456789",   "1b69b4bacd05f15",  "6664664565464057425",    "110110110100110110100101110101100110100000101111100010101");
	TEST_TOSTRING_ALL(unsigned long long, 9223372036854775808,  "9223372036854775808",  "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TEST_TOSTRING_ALL(unsigned long long, 18446744073709551615, "18446744073709551615", "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
}