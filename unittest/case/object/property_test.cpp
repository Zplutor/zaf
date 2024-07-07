#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/size.h>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>

namespace {

constexpr int ReadOnlyValue = 73;

class PropertyHost : public zaf::Object {
public:
    ZAF_OBJECT;

    int ReadWrite() const {
        return read_write_value_;
    }

    void SetReadWrite(int value) {
        read_write_value_ = value;
    }

    int ReadOnly() const {
        return ReadOnlyValue;
    }

    void SetWriteOnly(int value) {
        write_only_value_ = value;
    }

    int GetWriteOnlyValue() const {
        return write_only_value_;
    }

    float FloatType() const {
        return {};
    }

    std::wstring StringType() const {
        return {};
    }

    zaf::Size SizeType() const {
        return {};
    }

    RECT RectType() const {
        return {};
    }

    const std::shared_ptr<zaf::Point>& BoxedObject() const {
        return boxed_object_;
    }

    void SetBoxedObject(const std::shared_ptr<zaf::Point>& value) {
        boxed_object_ = value;
    }

    const std::shared_ptr<zaf::Image> Image() const {
        return image_;
    }

    void SetImage(const std::shared_ptr<zaf::Image>& image) {
        image_ = image;
    }

    std::optional<int> OptionalValue() const {
        return optional_value_;
    }

    void SetOptionalValue(std::optional<int> value) {
        optional_value_ = value;
    }

private:
    int read_write_value_{};
    int write_only_value_{};
    std::shared_ptr<zaf::Point> boxed_object_;
    std::shared_ptr<zaf::Image> image_;
    std::optional<int> optional_value_;
};

ZAF_OBJECT_BEGIN(PropertyHost)
ZAF_OBJECT_PROPERTY(ReadWrite);
ZAF_OBJECT_PROPERTY(ReadOnly);
ZAF_OBJECT_PROPERTY(WriteOnly);
ZAF_OBJECT_PROPERTY(FloatType);
ZAF_OBJECT_PROPERTY(StringType);
ZAF_OBJECT_PROPERTY(SizeType);
ZAF_OBJECT_PROPERTY(BoxedObject);
ZAF_OBJECT_PROPERTY(Image);
ZAF_OBJECT_PROPERTY(OptionalValue);
ZAF_OBJECT_END

ZAF_OBJECT_IMPL(PropertyHost);

}


TEST(PropertyTest, ReadWrite) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"ReadWrite");
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(property->Name(), L"ReadWrite");
    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueDynamic());

    property->SetValue(host, zaf::Box(19));
    auto boxed_value = property->GetValue(host);
    auto value = zaf::Unbox<int>(boxed_value);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(*value, 19);
}


TEST(PropertyTest, ReadOnly) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"ReadOnly");
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(property->Name(), L"ReadOnly");
    ASSERT_TRUE(property->CanGet());
    ASSERT_FALSE(property->CanSet());
    ASSERT_FALSE(property->IsValueDynamic());

    auto boxed_value = property->GetValue(host);
    auto value = zaf::Unbox<int>(boxed_value);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(*value, ReadOnlyValue);

    ASSERT_THROW(property->SetValue(host, zaf::Box(74)), zaf::InvalidOperationError);
}


TEST(PropertyTest, WriteOnly) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"WriteOnly");
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(property->Name(), L"WriteOnly");
    ASSERT_FALSE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueDynamic());

    property->SetValue(host, zaf::Box(76));
    ASSERT_EQ(host.GetWriteOnlyValue(), 76);

    ASSERT_THROW(property->GetValue(host), zaf::InvalidOperationError);
}


TEST(PropertyTest, NumericType) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"FloatType");
    ASSERT_NE(property, nullptr);

    auto type = property->ValueType();
    ASSERT_EQ(type->Name(), L"Float");
}


TEST(PropertyTest, StringType) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"StringType");
    ASSERT_NE(property, nullptr);

    auto type = property->ValueType();
    ASSERT_EQ(type->Name(), L"WideString");
}


TEST(PropertyTest, ObjectType) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"SizeType");
    ASSERT_NE(property, nullptr);

    auto type = property->ValueType();
    ASSERT_EQ(type->Name(), L"Size");
}


TEST(PropertyTest, BoxedObject) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"BoxedObject");
    ASSERT_NE(property, nullptr);

    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_TRUE(property->IsValueDynamic());

    auto set_value = zaf::Create<zaf::Point>(9.f, 8.f);
    property->SetValue(host, set_value);

    auto value = property->GetValue(host);
    ASSERT_NE(value, nullptr);
    ASSERT_TRUE(value->IsEqual(*set_value));
}


TEST(PropertyTest, Image) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"Image");
    ASSERT_NE(property, nullptr);

    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueDynamic());

    auto set_value = zaf::Create<zaf::URIImage>();
    property->SetValue(host, set_value);

    auto value = property->GetValue(host);
    ASSERT_NE(value, nullptr);
    ASSERT_TRUE(value->IsEqual(*set_value));
}


TEST(PropertyTest, OptionalValue) {

    PropertyHost host;
    auto property = host.DynamicType()->GetProperty(L"OptionalValue");
    ASSERT_NE(property, nullptr);

    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueDynamic());

    {
        auto set_value = zaf::Box(34);
        property->SetValue(host, set_value);

        ASSERT_EQ(host.OptionalValue(), 34);

        auto got_value = property->GetValue(host);
        ASSERT_NE(got_value, nullptr);
        ASSERT_TRUE(got_value->IsEqual(*set_value));
    }

    {
        property->SetValue(host, nullptr);
        ASSERT_EQ(host.OptionalValue(), std::nullopt);

        auto got_value = property->GetValue(host);
        ASSERT_EQ(got_value, nullptr);
    }
}


TEST(PropertyTest, ErrorValueType) {

    PropertyHost host;

    //Method property
    {
        auto property = host.DynamicType()->GetProperty(L"ReadWrite");
        auto frame = zaf::Create<zaf::Frame>();
        ASSERT_THROW(property->GetValue(*frame), zaf::InvalidTypeError);
        ASSERT_THROW(property->SetValue(host, frame), zaf::InvalidTypeError);
    }
}

