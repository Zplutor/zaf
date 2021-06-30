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
//Un-comment below line whould cause static assertion.
//ZAF_DEFINE_PROPERTY(RectType) 
ZAF_DEFINE_END

}


TEST(PropertyTest, ReadWrite) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"ReadWrite");
	ASSERT_NE(property, nullptr);
	ASSERT_EQ(property->GetName(), L"ReadWrite");

	property->SetValue(host, 19);
	auto value = std::any_cast<int>(property->GetValue(host));
	ASSERT_EQ(value, 19);
}


TEST(PropertyTest, ReadOnly) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"ReadOnly");
	ASSERT_NE(property, nullptr);
	ASSERT_EQ(property->GetName(), L"ReadOnly");

	auto value = std::any_cast<int>(property->GetValue(host));
	ASSERT_EQ(value, ReadOnlyValue);

	ASSERT_THROW(property->SetValue(host, 74), std::exception);
}


TEST(PropertyTest, WriteOnly) {

	PropertyHost host;
	auto property = host.GetType()->FindProperty(L"WriteOnly");
	ASSERT_NE(property, nullptr);
	ASSERT_EQ(property->GetName(), L"WriteOnly");

	property->SetValue(host, 76);
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