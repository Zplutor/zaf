#include <gtest/gtest.h>
#include <zaf/control/text_box.h>
#include <zaf/serialization/deserializing.h>

/*
TEST(TextBox, Deserialize) {

    auto json = "{ "
        "\"Type\": \"TextBox\","
        "\"Name\": \"textbox\", "
        "\"Text\": \"texttext\","
        "\"MaximumLength\": 44,"
        "\"UsePasswordCharacter\": true,"
        "\"IsMultiline\": true,"
        "\"IsReadOnly\": true,"
        "\"AllowBeep\": false,"
    "}";
    auto text_box = zaf::DeserializeObjectFromJson<zaf::TextBox>(json);
    ASSERT_NE(text_box, nullptr);
    ASSERT_EQ(text_box->GetName(), L"textbox");
    ASSERT_EQ(text_box->GetText(), L"texttext");
    ASSERT_EQ(text_box->GetMaximumLength(), 44);
    ASSERT_TRUE(text_box->UsePasswordCharacter());
    ASSERT_TRUE(text_box->IsMultiline());
    ASSERT_TRUE(text_box->IsReadOnly());
    ASSERT_FALSE(text_box->AllowBeep());
}
*/