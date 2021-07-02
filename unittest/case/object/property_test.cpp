#include <gtest/gtest.h>
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

	int int_field{};
	zaf::Size size_field{};
	RECT rect_field{};

private:
	int read_write_value_{};
	int write_only_value_{};
};

ZAF_DEFINE_TYPE(PropertyHost)
ZAF_DEFINE_PROPERTY(ReadWrite)
ZAF_DEFINE_PROPERTY(ReadOnly)
ZAF_DEFINE_PROPERTY(WriteOnly)
ZAF_DEFINE_PROPERTY(FloatType)
ZAF_DEFINE_PROPERTY(StringType)
ZAF_DEFINE_PROPERTY(SizeType)
ZAF_DEFINE_PROPERTY_WITH_FIELD(IntField, int_field)
ZAF_DEFINE_PROPERTY_WITH_FIELD(SizeField, size_field)
//Un-comment below lines whould cause static assertion.
//ZAF_DEFINE_PROPERTY(RectType) 
//ZAF_DEFINE_PROPERTY_WITH_FIELD(RectField, rect_field)
ZAF_DEFINE_END

}


TEST(PropertyTest, ReadWrite) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"ReadWrite");
	ASSERT_NE(property, nullptr);
	ASSERT_EQ(property->GetName(), L"ReadWrite");
	ASSERT_TRUE(property->CanGet());
	ASSERT_TRUE(property->CanSet());

	property->SetValue(host, zaf::Box(19));
	auto boxed_value = property->GetValue(host);
	auto value = zaf::TryUnbox<int>(boxed_value);
	ASSERT_NE(value, nullptr);
	ASSERT_EQ(*value, 19);
}


TEST(PropertyTest, ReadOnly) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"ReadOnly");
	ASSERT_NE(property, nullptr);
	ASSERT_EQ(property->GetName(), L"ReadOnly");
	ASSERT_TRUE(property->CanGet());
	ASSERT_FALSE(property->CanSet());

	auto boxed_value = property->GetValue(host);
	auto value = zaf::TryUnbox<int>(boxed_value);
	ASSERT_NE(value, nullptr);
	ASSERT_EQ(*value, ReadOnlyValue);

	ASSERT_THROW(property->SetValue(host, zaf::Box(74)), std::exception);
}


TEST(PropertyTest, WriteOnly) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"WriteOnly");
	ASSERT_NE(property, nullptr);
	ASSERT_EQ(property->GetName(), L"WriteOnly");
	ASSERT_FALSE(property->CanGet());
	ASSERT_TRUE(property->CanSet());

	property->SetValue(host, zaf::Box(76));
	ASSERT_EQ(host.GetWriteOnlyValue(), 76);

	ASSERT_THROW(property->GetValue(host), std::exception);
}


TEST(PropertyTest, NumericType) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"FloatType");
	ASSERT_NE(property, nullptr);

	auto type = property->GetValueType();
	ASSERT_EQ(type->GetName(), L"Float");
}


TEST(PropertyTest, StringType) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"StringType");
	ASSERT_NE(property, nullptr);

	auto type = property->GetValueType();
	ASSERT_EQ(type->GetName(), L"WideString");
}


TEST(PropertyTest, ObjectType) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"SizeType");
	ASSERT_NE(property, nullptr);

	auto type = property->GetValueType();
	ASSERT_EQ(type->GetName(), L"Size");
}


TEST(PropertyTest, NumericTypeField) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"IntField");
	ASSERT_NE(property, nullptr);
	ASSERT_TRUE(property->CanGet());
	ASSERT_TRUE(property->CanSet());

	property->SetValue(host, zaf::Box(938));

	auto boxed_value = property->GetValue(host);
	auto value = zaf::TryUnbox<int>(boxed_value);
	ASSERT_NE(value, nullptr);
	ASSERT_EQ(*value, 938);
}


TEST(PropertyTest, ObjectTypeField) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"SizeField");
	ASSERT_NE(property, nullptr);
	ASSERT_TRUE(property->CanGet());
	ASSERT_TRUE(property->CanSet());

	property->SetValue(host, zaf::Size{ 78, 87 });

	auto boxed_value = property->GetValue(host);
	auto value = zaf::TryUnbox<zaf::Size>(boxed_value);
	ASSERT_NE(value, nullptr);
	ASSERT_EQ(*value, zaf::Size(78, 87));
}