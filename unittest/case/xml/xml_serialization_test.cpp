#include <gtest/gtest.h>
#include <zaf/control/textual/styled_text.h>
#include <zaf/xml/xml_serialization.h>

using namespace zaf;
using namespace zaf::textual;

TEST(XMLSerializationTest, SerializeObjectToText) {

    StyledText text;
    auto xml_text = XMLSerializeToText(text);
}


TEST(XMLSerializationTest, DeserializeObjectFromText) {

    Range range;

    XMLDeserializeFromText(L"<?xml><Range Index=\"0\" Length=\"1\" / >", range);
}