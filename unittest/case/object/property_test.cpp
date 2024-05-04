#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/size.h>
#include <zaf/object/object.h>
#include <zaf/object/type_definition.h>

namespace {

constexpr int ReadOnlyValue = 73;

class PropertyHost : public zaf::Object {
public:
    ZAF_DECLARE_TYPE;

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

private:
    int read_write_value_{};
    int write_only_value_{};
    std::shared_ptr<zaf::Point> boxed_object_;
    std::shared_ptr<zaf::Image> image_;
};

ZAF_DEFINE_TYPE(PropertyHost)
ZAF_DEFINE_TYPE_PROPERTY(ReadWrite)
ZAF_DEFINE_TYPE_PROPERTY(ReadOnly)
ZAF_DEFINE_TYPE_PROPERTY(WriteOnly)
ZAF_DEFINE_TYPE_PROPERTY(FloatType)
ZAF_DEFINE_TYPE_PROPERTY(StringType)
ZAF_DEFINE_TYPE_PROPERTY(SizeType)
ZAF_DEFINE_TYPE_PROPERTY(BoxedObject);
ZAF_DEFINE_TYPE_PROPERTY(Image);
//Un-comment below lines whould cause static assertion.
//ZAF_DEFINE_TYPE_PROPERTY(RectType) 
ZAF_DEFINE_TYPE_END

}


TEST(PropertyTest, ReadWrite) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"ReadWrite");
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(property->GetName(), L"ReadWrite");
    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueTypeDynamic());

    property->SetValue(host, zaf::Box(19));
    auto boxed_value = property->GetValue(host);
    auto value = zaf::Unbox<int>(boxed_value);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(*value, 19);
}


TEST(PropertyTest, ReadOnly) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"ReadOnly");
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(property->GetName(), L"ReadOnly");
    ASSERT_TRUE(property->CanGet());
    ASSERT_FALSE(property->CanSet());
    ASSERT_FALSE(property->IsValueTypeDynamic());

    auto boxed_value = property->GetValue(host);
    auto value = zaf::Unbox<int>(boxed_value);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(*value, ReadOnlyValue);

    ASSERT_THROW(property->SetValue(host, zaf::Box(74)), zaf::InvalidOperationError);
}


TEST(PropertyTest, WriteOnly) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"WriteOnly");
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(property->GetName(), L"WriteOnly");
    ASSERT_FALSE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueTypeDynamic());

    property->SetValue(host, zaf::Box(76));
    ASSERT_EQ(host.GetWriteOnlyValue(), 76);

    ASSERT_THROW(property->GetValue(host), zaf::InvalidOperationError);
}


TEST(PropertyTest, NumericType) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"FloatType");
    ASSERT_NE(property, nullptr);

    auto type = property->GetValueType();
    ASSERT_EQ(type->GetName(), L"Float");
}


TEST(PropertyTest, StringType) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"StringType");
    ASSERT_NE(property, nullptr);

    auto type = property->GetValueType();
    ASSERT_EQ(type->GetName(), L"WideString");
}


TEST(PropertyTest, ObjectType) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"SizeType");
    ASSERT_NE(property, nullptr);

    auto type = property->GetValueType();
    ASSERT_EQ(type->GetName(), L"Size");
}


TEST(PropertyType, BoxedObject) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"BoxedObject");
    ASSERT_NE(property, nullptr);

    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueTypeDynamic());

    auto set_value = zaf::Create<zaf::Point>(9.f, 8.f);
    property->SetValue(host, set_value);

    auto value = property->GetValue(host);
    ASSERT_NE(value, nullptr);
    ASSERT_TRUE(value->IsEqual(*set_value));
}


TEST(PropertyType, Image) {

    PropertyHost host;
    auto property = host.GetType()->GetProperty(L"Image");
    ASSERT_NE(property, nullptr);

    ASSERT_TRUE(property->CanGet());
    ASSERT_TRUE(property->CanSet());
    ASSERT_FALSE(property->IsValueTypeDynamic());

    auto set_value = zaf::Create<zaf::URIImage>();
    property->SetValue(host, set_value);

    auto value = property->GetValue(host);
    ASSERT_NE(value, nullptr);
    ASSERT_TRUE(value->IsEqual(*set_value));
}


TEST(PropertyTest, ErrorValueType) {

    PropertyHost host;

    //Method property
    {
        auto property = host.GetType()->GetProperty(L"ReadWrite");
        auto frame = zaf::Create<zaf::Frame>();
        ASSERT_THROW(property->GetValue(*frame), zaf::InvalidTypeError);
        ASSERT_THROW(property->SetValue(host, frame), zaf::InvalidTypeError);
    }
}

