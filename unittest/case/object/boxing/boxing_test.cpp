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


TEST(BoxingTest, TryUnboxBuiltInBoxedType) {

    std::string string{ "String" };
    auto boxed_string = Box(string);

    //std::shared_ptr overload
    auto unboxed_string = TryUnbox<std::string>(boxed_string);
    ASSERT_NE(unboxed_string, nullptr);
    ASSERT_EQ(*unboxed_string, string);

    //non-const reference overload
    unboxed_string = TryUnbox<std::string>(*boxed_string);
    ASSERT_NE(unboxed_string, nullptr);
    ASSERT_EQ(*unboxed_string, string);

    //const reference overload
    auto const_unboxed_string = TryUnbox<std::string>(static_cast<const String&>(*boxed_string));
    ASSERT_NE(const_unboxed_string, nullptr);
    ASSERT_EQ(*const_unboxed_string, string);
}


TEST(BoxingTest, TryUnboxObjectType) {

    Rect rect{ 11, 11, 22, 22 };
    std::shared_ptr<Rect> boxed_rect = Box(rect);

    //std::shared_ptr overload
    auto unboxed_rect = TryUnbox<Rect>(boxed_rect);
    ASSERT_NE(unboxed_rect, nullptr);
    ASSERT_EQ(*unboxed_rect, rect);

    //non-const reference overload
    unboxed_rect = TryUnbox<Rect>(*boxed_rect);
    ASSERT_NE(unboxed_rect, nullptr);
    ASSERT_EQ(*unboxed_rect, rect);

    //const reference overload
    auto const_unboxed_rect = TryUnbox<Rect>(static_cast<const Rect&>(*boxed_rect));
    ASSERT_NE(const_unboxed_rect, nullptr);
    ASSERT_EQ(*const_unboxed_rect, rect);
}


TEST(BoxingTest, TryUnboxOtherType) {

    std::vector<int> vector{ 3, 4, 5 };
    auto boxed_vector = Box(vector);

    //std::shared_ptr overload
    auto unboxed_vector = TryUnbox<std::vector<int>>(boxed_vector);
    ASSERT_NE(unboxed_vector, nullptr);
    ASSERT_EQ(*unboxed_vector, vector);

    //non-const reference overload
    unboxed_vector = TryUnbox<std::vector<int>>(*boxed_vector);
    ASSERT_NE(unboxed_vector, nullptr);
    ASSERT_EQ(*unboxed_vector, vector);

    //const reference overload
    auto const_unboxed_vector = 
        TryUnbox<std::vector<int>>(
            static_cast<const BoxedObject<std::vector<int>>&>(*boxed_vector));
    ASSERT_NE(unboxed_vector, nullptr);
    ASSERT_EQ(*unboxed_vector, vector);
}


TEST(BoxingTest, TryUnboxFail) {

    auto boxed = Box(1);
    auto unboxed = TryUnbox<std::string>(boxed);
    ASSERT_EQ(unboxed, nullptr);
}


TEST(BoxingTest, UnboxBuiltInType) {

    std::wstring string{ L"String" };
    auto boxed_string = Box(string);

    //std::shared_ptr overload
    auto& unboxed_string = Unbox<std::wstring>(boxed_string);
    ASSERT_EQ(unboxed_string, string);

    //non-const reference overload
    auto& unboxed_string2 = Unbox<std::wstring>(*boxed_string);
    ASSERT_EQ(unboxed_string2, string);

    //const reference overload
    auto& const_unboxed_string = Unbox<std::wstring>(static_cast<const WideString&>(*boxed_string));
    ASSERT_EQ(const_unboxed_string, string);
}


TEST(BoxingTest, UnboxObjectType) {

    Point point{ 11, 22 };
    std::shared_ptr<Point> boxed = Box(point);

    //std::shared_ptr overload
    auto& unboxed = Unbox<Point>(boxed);
    ASSERT_EQ(unboxed, point);

    //non-const reference overload
    auto& unboxed2 = Unbox<Point>(*boxed);
    ASSERT_EQ(unboxed, point);

    //const reference overload
    auto& const_unboxed = Unbox<Point>(static_cast<const Point&>(*boxed));
    ASSERT_EQ(const_unboxed, point);
}


TEST(BoxingTest, UnboxOtherType) {

    std::list<int> list{ 6, 7, 8 };
    auto boxed = Box(list);

    //std::shared_ptr overload
    auto& unboxed = Unbox<std::list<int>>(boxed);
    ASSERT_EQ(unboxed, list);

    //non-const reference overload
    auto& unboxed2 = Unbox<std::list<int>>(*boxed);
    ASSERT_EQ(unboxed, list);

    //const reference overload
    auto& const_unboxed =
        Unbox<std::list<int>>(
            static_cast<const BoxedObject<std::list<int>>&>(*boxed));
    ASSERT_EQ(const_unboxed, list);
}


TEST(BoxingTest, UnboxFail) {

    auto boxed = Box(100.f);
    ASSERT_THROW(Unbox<std::string>(boxed), std::bad_cast);
}