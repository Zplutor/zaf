#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/base/non_copyable.h>
#include <zaf/graphic/rect.h>
#include <zaf/dynamic/boxing/boxing.h>
#include <zaf/dynamic/boxing/string.h>

using namespace zaf;

namespace {

class NonCopyableValue : NonCopyable {
public:
    std::string string{};
};

class NonCopyableObject {
public:
    explicit NonCopyableObject(NonCopyableValue value) : Value(std::move(value))  {

    }

    NonCopyableValue Value;
};

template<>
struct zaf__CustomBoxingTraits<NonCopyableValue> {

    using BoxedType = NonCopyableObject;

    static std::shared_ptr<NonCopyableObject> Box(NonCopyableValue value) {
        return std::make_shared<NonCopyableObject>(std::move(value));
    }

    static const NonCopyableValue* Unbox(const NonCopyableObject& object) {
        return &object.Value;
    }
};

}

TEST(BoxingTest, Box) {

    //Reflective type
    {
        Rect rect{ 10, 10, 20, 20 };
        std::shared_ptr<Rect> boxed_rect = Box(rect);
        ASSERT_EQ(*boxed_rect, rect);
    }

    //Const reflective type
    {
        const Rect rect{ 50, 40, 70, 80 };
        std::shared_ptr<Rect> boxed_rect = Box(rect);
        ASSERT_EQ(*boxed_rect, rect);
    }

    //Reference to const reflective type
    {
        const Rect rect{ 50, 40, 70, 80 };
        const Rect& ref = rect;
        std::shared_ptr<Rect> boxed_rect = Box(ref);
        ASSERT_EQ(*boxed_rect, rect);
    }

    //Custom boxing type
    {
        std::string string{ "String" };
        std::shared_ptr<String> boxed_string = Box(string);
        ASSERT_EQ(boxed_string->Value(), string);
    }

    //Const custom boxing type
    {
        const std::string string{ "Const String" };
        std::shared_ptr<String> boxed_string = Box(string);
        ASSERT_EQ(boxed_string->Value(), string);
    }

    //Reference to const custom boxing type
    {
        const std::string string{ "Const Reference String" };
        const std::string& ref = string;
        std::shared_ptr<String> boxed_string = Box(ref);
        ASSERT_EQ(boxed_string->Value(), string);
    }

    //Non copyable value 
    {
        NonCopyableValue value;
        value.string = "non-copyable";
        std::shared_ptr<NonCopyableObject> object = Box(std::move(value));
        ASSERT_EQ(object->Value.string, "non-copyable");
        ASSERT_EQ(value.string, "");
    }
}


TEST(BoxingTest, UnboxPointer) {

    //Null pointer
    {
        dynamic::Object* boxed_object{};
        auto unboxed_bool = Unbox<bool>(boxed_object);
        ASSERT_EQ(unboxed_bool, nullptr);
    }

    //Const null pointer
    {
        const dynamic::Object* boxed_object{};
        auto unboxed_bool = Unbox<bool>(boxed_object);
        ASSERT_EQ(unboxed_bool, nullptr);
    }

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

    //null std::shared_ptr
    {
        std::shared_ptr<dynamic::Object> object;
        auto unboxed_bool = Unbox<bool>(object);
        ASSERT_EQ(unboxed_bool, nullptr);
    }

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


TEST(BoxingTest, StringBoxing) {

    {
        std::string string{ "string" };
        std::shared_ptr<String> boxed_string = Box(string);
        ASSERT_EQ(boxed_string->Value(), string);
    }

    {
        std::string_view string_view{ "string_view" };
        std::shared_ptr<String> boxed_string = Box(string_view);
        ASSERT_EQ(boxed_string->Value(), string_view);
    }

    {
        char* char_pointer = const_cast<char*>("char*");
        std::shared_ptr<String> boxed_string = Box(char_pointer);
        ASSERT_STREQ(boxed_string->Value().c_str(), char_pointer);
    }

    {
        const char* char_pointer = "const char*";
        std::shared_ptr<String> boxed_string = Box(char_pointer);
        ASSERT_STREQ(boxed_string->Value().c_str(), char_pointer);
    }
}


TEST(BoxingTest, StringUnboxing) {

    String boxed_string{ "boxed-string" };
    std::string& string = Unbox<std::string>(boxed_string);
    ASSERT_EQ(&string, &boxed_string.Value());
}


TEST(BoxingTest, WideStringBoxing) {

    {
        std::wstring string{ L"string" };
        std::shared_ptr<WideString> boxed_string = Box(string);
        ASSERT_EQ(boxed_string->Value(), string);
    }

    {
        std::wstring_view string_view{ L"wstring_view" };
        std::shared_ptr<WideString> boxed_string = Box(string_view);
        ASSERT_EQ(boxed_string->Value(), string_view);
    }

    {
        wchar_t* char_pointer = const_cast<wchar_t*>(L"wchar_t*");
        std::shared_ptr<WideString> boxed_string = Box(char_pointer);
        ASSERT_STREQ(boxed_string->Value().c_str(), char_pointer);
    }

    {
        const wchar_t* char_pointer = L"const wchar_t*";
        std::shared_ptr<WideString> boxed_string = Box(char_pointer);
        ASSERT_STREQ(boxed_string->Value().c_str(), char_pointer);
    }
}


TEST(BoxingTest, WideStringUnboxing) {

    WideString boxed_string{ L"boxed-string" };
    std::wstring& string = Unbox<std::wstring>(boxed_string);
    ASSERT_EQ(&string, &boxed_string.Value());
}