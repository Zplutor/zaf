#include <gtest/gtest.h>
#include <zaf/graphic/rect.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/boxing/string.h>

using namespace zaf;

TEST(BoxingTest, Box) {

    std::string string{ "String" };
    std::shared_ptr<String> boxed_string = Box(string);
    ASSERT_EQ(boxed_string->Value(), string);

    const char* chars = "Chars";
    boxed_string = Box(chars);
    ASSERT_EQ(boxed_string->Value(), chars);

    std::wstring wide_string{ L"WideString" };
    std::shared_ptr<WideString> boxed_wide_string = Box(wide_string);
    ASSERT_EQ(boxed_wide_string->Value(), wide_string);

    const wchar_t* wide_chars = L"WideChars";
    boxed_wide_string = Box(wide_chars);
    ASSERT_EQ(boxed_wide_string->Value(), wide_chars);

    std::vector<int> vector{ 0, 1, 2 };
    std::shared_ptr<BoxedObject<std::vector<int>>> boxed_vector = Box(vector);
    ASSERT_EQ(boxed_vector->Value(), vector);

    Rect rect{ 10, 10, 20, 20 };
    std::shared_ptr<Rect> boxed_rect = Box(rect);
    ASSERT_EQ(*boxed_rect, rect);
}


TEST(BoxingTest, Unbox) {

    std::string string{ "String" };
    auto boxed_string = Box(string);
    auto unboxed_string = TryUnbox<std::string>(boxed_string);
    ASSERT_NE(unboxed_string, nullptr);
    ASSERT_EQ(*unboxed_string, string);

    auto wide_string = TryUnbox<std::wstring>(boxed_string);
    ASSERT_EQ(wide_string, nullptr);

    std::vector<int> vector{ 3, 4, 5 };
    auto boxed_vector = Box(vector);
    auto unboxed_vector = TryUnbox<std::vector<int>>(boxed_vector);
    ASSERT_NE(unboxed_vector, nullptr);
    ASSERT_EQ(*unboxed_vector, vector);

    Rect rect{ 11, 11, 22, 22 };
    std::shared_ptr<Rect> boxed_rect = Box(rect);
    auto unboxed_rect = TryUnbox<Rect>(boxed_rect);
    ASSERT_NE(unboxed_rect, nullptr);
    ASSERT_EQ(*unboxed_rect, rect);
}