#include <gtest/gtest.h>
#include <zaf/base/range.h>
#include <zaf/base/error/com_error.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_serialization.h>

using namespace zaf;

TEST(XMLSerializationTest, XMLSerializeToText) {

    Range range;
    auto xml = XMLSerializeToText(range);

    auto expected = 
        LR"(<?xml version="1.0" encoding="UTF-16" standalone="yes"?>)"
        LR"(<Range Index="0" Length="0" />)";

    ASSERT_EQ(xml, expected);
}


TEST(XMLSerializationTest, XMLDeserializeFromText) {

    {
        Range range;
        XMLDeserialize(
            range,
            LR"(<?xml version="1.0" encoding="UTF-16"?><Range Index="1" Length="10"/>)");

        ASSERT_EQ(range.index, 1);
        ASSERT_EQ(range.length, 10);
    }

    //There are white spaces at the end of the XML document.
    {
        Range range;
        XMLDeserialize(
            range,
            LR"(<?xml version="1.0" encoding="UTF-16"?><Range Index="1" Length="10"/>  )");

        ASSERT_EQ(range.index, 1);
        ASSERT_EQ(range.length, 10);
    }

    //There are redundant content at the end of the XML document.
    {
        Range range;
        ASSERT_THROW(
            XMLDeserialize(
                range,
                LR"(<?xml version="1.0" encoding="UTF-16"?>)"
                LR"(<Range Index="1" Length="10" /><Range />)"),
            COMError);
    }

    //There is no XML declaration.
    {
        Range range;
        ASSERT_THROW(XMLDeserialize(range, LR"(<Range Index="3" Length="1" />)"), XMLError);
    }
}