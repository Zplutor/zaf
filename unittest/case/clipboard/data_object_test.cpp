#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/clipboard/data_object.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/clipboard/unknown_data.h>
#include <zaf/object/type_definition.h>

using namespace zaf::clipboard;

namespace {

class TestClipboardData : public ClipboardData {
public:
    ZAF_DECLARE_TYPE;

    Medium SaveToMedium(const DataDescriptor& format) override {
        return {};
    }

    void LoadFromMedium(const Medium& medium, const DataDescriptor& data_descriptor) override {

    }
};

ZAF_DEFINE_TYPE(TestClipboardData);
ZAF_DEFINE_TYPE_END;

}


TEST(DataObjectTest, Construct) {

    //DataObject created with the default constructor is invalid.
    {
        DataObject data_object;
        ASSERT_FALSE(data_object);
    }

    //DataObject created with Create() is valid.
    {
        DataObject data_object = DataObject::Create();
        ASSERT_TRUE(data_object);
    }
}


TEST(DataObjectTest, SetAndGetText) {

    constexpr const wchar_t* TestText = L"DataObjectTestText";

    auto data_object = DataObject::Create();
    data_object.SetText(TestText);
    auto result = data_object.GetText();
    ASSERT_EQ(result, TestText);
}


TEST(DataObjectTest, GetUnknownData) {

    auto format_type = MakePrivateFormatType(78);

    auto data_object = DataObject::Create();
    data_object.SetData(
        std::make_shared<TestClipboardData>(), 
        DataDescriptor::FromFormatType(format_type));

    auto got_data = data_object.GetData(DataDescriptor::FromFormatType(format_type));
    ASSERT_NE(got_data, nullptr);
    auto unknown_data = zaf::As<UnknownData>(got_data);
    ASSERT_NE(unknown_data, nullptr);
}


TEST(DataObjectTest, GetRegisteredData) {

    auto format_type = MakePrivateFormatType(1);
    Clipboard::RegisterClipboardData(format_type, TestClipboardData::Type);

    auto data_object = DataObject::Create();
    data_object.SetData(
        std::make_shared<TestClipboardData>(), 
        DataDescriptor::FromFormatType(format_type));

    auto got_data = data_object.GetData(DataDescriptor::FromFormatType(format_type));
    ASSERT_NE(got_data, nullptr);
    auto unknown_data = zaf::As<TestClipboardData>(got_data);
    ASSERT_NE(unknown_data, nullptr);
}