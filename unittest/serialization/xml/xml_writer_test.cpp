#include <gtest/gtest.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/xml/xml_writer.h>

namespace {

std::string ConvertDataNodeToXmlString(const std::shared_ptr<zaf::DataNode>& data_node) {

    auto stream = zaf::CreateMemoryStream();

    zaf::XmlWriter writer;
    std::error_code error_code;
    writer.Execute(data_node, stream, error_code);

    auto length = stream.GetLength();
    std::string string;
    string.resize(static_cast<std::size_t>(length));

    stream.Seek(zaf::Stream::Origin::Set, 0);
    stream.Read(static_cast<std::size_t>(length), &string[0]);
    return string;
}

}


TEST(XmlWriter, WriteStringNode) {

    auto string_node = zaf::DataNode::CreateString(L"zaf");
    auto xml_string = ConvertDataNodeToXmlString(string_node);
    ASSERT_EQ(xml_string, "<string>zaf</string>");
}


TEST(XmlWriter, WriteBooleanNode) {

    auto boolean_node = zaf::DataNode::CreateBoolean(true);
    auto xml_string = ConvertDataNodeToXmlString(boolean_node);
    ASSERT_EQ(xml_string, "<boolean>true</boolean>");

    boolean_node = zaf::DataNode::CreateBoolean(false);
    xml_string = ConvertDataNodeToXmlString(boolean_node);
    ASSERT_EQ(xml_string, "<boolean>false</boolean>");
}


TEST(XmlWriter, WriteNumberNode) {

    auto number_node = zaf::DataNode::CreateNumber(0);
    auto xml_string = ConvertDataNodeToXmlString(number_node);
    ASSERT_EQ(xml_string, "<number>0</number>");

    number_node = zaf::DataNode::CreateNumber(11);
    xml_string = ConvertDataNodeToXmlString(number_node);
    ASSERT_EQ(xml_string, "<number>11</number>");

    number_node = zaf::DataNode::CreateNumber(-11);
    xml_string = ConvertDataNodeToXmlString(number_node);
    ASSERT_EQ(xml_string, "<number>-11</number>");
}


TEST(XmlWriter, WriteObjectNode) {

    auto node = zaf::DataNode::CreateObject();
    auto xml_string = ConvertDataNodeToXmlString(node);
    ASSERT_EQ(xml_string, "<object />");

    node->AddField(L"StringField", zaf::DataNode::CreateString(L"zaf"));
    node->AddField(L"BooleanField", zaf::DataNode::CreateBoolean(true));
    node->AddField(L"NumberField", zaf::DataNode::CreateNumber(44));
    node->AddField(L"ObjectField", zaf::DataNode::CreateObject());
    node->AddField(L"ArrayField", zaf::DataNode::CreateArray());

    xml_string = ConvertDataNodeToXmlString(node);
    const char* expected_string =
        "<object>"
        "<string key=\"StringField\">zaf</string>"
        "<boolean key=\"BooleanField\">true</boolean>"
        "<number key=\"NumberField\">44</number>"
        "<object key=\"ObjectField\" />"
        "<array key=\"ArrayField\" />"
        "</object>";
    ASSERT_EQ(xml_string, expected_string);
}


TEST(XmlWriter, WriteArrayNode) {

    auto node = zaf::DataNode::CreateArray();
    auto xml_string = ConvertDataNodeToXmlString(node);
    ASSERT_EQ(xml_string, "<array />");

    node->AddElement(zaf::DataNode::CreateString(L"zaf"));
    node->AddElement(zaf::DataNode::CreateBoolean(true));
    node->AddElement(zaf::DataNode::CreateNumber(11));
    node->AddElement(zaf::DataNode::CreateObject());
    node->AddElement(zaf::DataNode::CreateArray());

    xml_string = ConvertDataNodeToXmlString(node);
    const char* expected_string = 
        "<array>"
        "<string>zaf</string>"
        "<boolean>true</boolean>"
        "<number>11</number>"
        "<object />"
        "<array />"
        "</array>";
    ASSERT_EQ(xml_string, expected_string);
}