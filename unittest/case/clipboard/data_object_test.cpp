#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/clipboard/data_object.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/object/type_definition.h>

using namespace zaf::clipboard;

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