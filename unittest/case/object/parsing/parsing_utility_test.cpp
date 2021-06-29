#include <gtest/gtest.h>
#include <zaf/graphic/point.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/utility.h>

TEST(ParsingUtilityTest, GetObjectParserByNumericType) {

	auto parser = zaf::GetObjectParser<int>();
	ASSERT_NE(parser, nullptr);

	zaf::Int32 i32;
	parser->ParseFromAttribute(L"111", i32);
	ASSERT_EQ(i32.Value(), 111);
}


TEST(ParsingUtilityTest, GetObjectParserByStringType) {

	auto parser = zaf::GetObjectParser<const char*>();
	ASSERT_NE(parser, nullptr);

	zaf::String string;
	parser->ParseFromAttribute(L"sss", string);
	ASSERT_EQ(string.Value(), "sss");
}


TEST(ParsingUtilityTest, GetObjectParserByObjectType) {

	auto parser = zaf::GetObjectParser<zaf::Point>();
	ASSERT_NE(parser, nullptr);

	zaf::Point point;
	parser->ParseFromAttribute(L"10,20", point);
	ASSERT_EQ(point, zaf::Point(10, 20));
}