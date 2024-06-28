#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/graphic/rect.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/boxing/string.h>

using namespace zaf;

TEST(BoxingTest, Box) {

    std::string string{ "String" };
    std::shared_ptr<String> boxed_string = Box(string);
    ASSERT_EQ(boxed_string->Value(), string);

    std::wstring wide_string{ L"WideString" };
    std::shared_ptr<WideString> boxed_wide_string = Box(wide_string);
    ASSERT_EQ(boxed_wide_string->Value(), wide_string);

    Rect rect{ 10, 10, 20, 20 };
    std::shared_ptr<Rect> boxed_rect = Box(rect);
    ASSERT_EQ(*boxed_rect, rect);
}


TEST(BoxingTest, UnboxPointer) {

    //Non-const pointer
    {
        std::shared_ptr<Boolean> boxed = Box(true);

        static_assert(std::is_same_v<bool*, decltype(Unbox<bool>(boxed.get()))>);

        bool* unboxed = Unbox<bool>(boxed.get());
        ASSERT_NE(unboxed, nullptr);
        ASSERT_EQ(*unboxed, true);

        //Unbox failed
        int* failed = Unbox<int>(boxed.get());
        ASSERT_EQ(failed, nullptr);
    }

    //Const pointer
    {
        std::shared_ptr<const Boolean> const_boxed = Box(true);

        static_assert(std::is_same_v<const bool*, decltype(Unbox<bool>(const_boxed.get()))>);

        const bool* unboxed = Unbox<bool>(const_boxed.get());
        ASSERT_NE(unboxed, nullptr);
        ASSERT_EQ(*unboxed, true);

        //Unbox failed
        const int* failed = Unbox<int>(const_boxed.get());
        ASSERT_EQ(failed, nullptr);
    }
}


TEST(BoxingTest, UnboxSharedPtr) {

    //std::shared_ptr
    {
        std::shared_ptr<Float> boxed = Box(44.3f);

        static_assert(std::is_same_v<float*, decltype(Unbox<float>(boxed))>);

        float* unboxed = Unbox<float>(boxed);
        ASSERT_NE(unboxed, nullptr);
        ASSERT_EQ(*unboxed, 44.3f);
    }

    //Unbox failed
    {
        std::shared_ptr<Float> boxed = Box(45.6f);

        double* unboxed = Unbox<double>(boxed);
        ASSERT_EQ(unboxed, nullptr);
    }
}


TEST(BoxingTest, UnboxReference) {

    //Non-const reference
    {
        std::shared_ptr<String> boxed = Box(std::string{ "bbbbox"});

        static_assert(std::is_same_v<std::string&, decltype(Unbox<std::string>(*boxed))>);

        std::string& unboxed = Unbox<std::string>(*boxed);
        ASSERT_EQ(unboxed, "bbbbox");

        //Unbox failed
        ASSERT_THROW(Unbox<std::wstring>(*boxed), zaf::InvalidTypeError);
    }

    //Const reference 
    {
        std::shared_ptr<const String> boxed = Box(std::string{ "aaabb" });

        static_assert(std::is_same_v<const std::string&, decltype(Unbox<std::string>(*boxed))>);

        const std::string& unboxed = Unbox<std::string>(*boxed);
        ASSERT_EQ(unboxed, "aaabb");

        //Unbox failed
        ASSERT_THROW(Unbox<std::wstring>(*boxed), zaf::InvalidTypeError);
    }
}


TEST(BoxingTest, UnboxObjectType) {

    Rect rect{ 11, 11, 22, 22 };
    std::shared_ptr<Rect> boxed_rect = Box(rect);

    //Non-const pointer overload 
    {
        Rect* unboxed_rect = Unbox<Rect>(boxed_rect.get());
        ASSERT_NE(unboxed_rect, nullptr);
        ASSERT_EQ(*unboxed_rect, rect);
    }

    //Const pointer overload
    {
        const Rect* unboxed_rect = Unbox<Rect>(static_cast<const Rect*>(boxed_rect.get()));
        ASSERT_NE(unboxed_rect, nullptr);
        ASSERT_EQ(*unboxed_rect, rect);
    }

    //std::shared_ptr overload
    {
        auto unboxed_rect = Unbox<Rect>(boxed_rect);
        ASSERT_NE(unboxed_rect, nullptr);
        ASSERT_EQ(*unboxed_rect, rect);
    }

    //non-const reference overload
    {
        Rect& unboxed_rect = Unbox<Rect>(*boxed_rect);
        ASSERT_EQ(unboxed_rect, rect);
    }

    //const reference overload
    {
        const Rect& const_unboxed_rect = Unbox<Rect>(static_cast<const Rect&>(*boxed_rect));
        ASSERT_EQ(const_unboxed_rect, rect);
    }
}