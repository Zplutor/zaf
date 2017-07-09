#include <gtest/gtest.h>
#include <zaf/control/list_box.h>
#include "utility/deserialize_utility.h"

TEST(ListBox, Deserialize) {

    auto json = "{\"Name\":\"ListBox\", \"ItemHeight\":44}";
    auto control = DeserializeObject<zaf::ListBox>(json);
    ASSERT_EQ(control->GetName(), L"ListBox");
    ASSERT_EQ(control->GetItemHeight(), 44.f);
}


TEST(ListBox, DeserializeItems) {

    auto json = "{\"Items\": []}";
    auto control = DeserializeObject<zaf::ListBox>(json);
    ASSERT_EQ(control->GetItemCount(), 0);

    json = "{"
    "\"Items\":["
    "\"aaa\","
    "\"bbb\","
    "\"ccc\","
    "\"ddd\","
    "]"
    "}";
    control = DeserializeObject<zaf::ListBox>(json);
    ASSERT_EQ(control->GetItemCount(), 4);
    ASSERT_EQ(control->GetItemTextAtIndex(0), L"aaa");
    ASSERT_EQ(control->GetItemTextAtIndex(1), L"bbb");
    ASSERT_EQ(control->GetItemTextAtIndex(2), L"ccc");
    ASSERT_EQ(control->GetItemTextAtIndex(3), L"ddd");
}