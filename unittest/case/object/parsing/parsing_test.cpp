#include <gtest/gtest.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/object/object.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/type_definition.h>

namespace {

class Base : public zaf::Object {
public:
    ZAF_DECLARE_TYPE

public:
    int base_value{};
};

class BaseParser : public zaf::ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, zaf::Object& object) override {

    }

    void ParseFromNode(const zaf::XamlNode& node, zaf::Object& object) override {

        auto base_value = node.GetAttribute(L"BaseValue");
        if (base_value) {
            dynamic_cast<Base&>(object).base_value = zaf::ToNumeric<int>(*base_value);
        }
    }
};

ZAF_DEFINE_TYPE(Base)
ZAF_DEFINE_TYPE_PARSER(BaseParser)
ZAF_DEFINE_TYPE_END


class Derived : public Base {
public:
    ZAF_DECLARE_TYPE

public:
    int derived_value{};
};

ZAF_DEFINE_TYPE(Derived)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(DerivedValue, derived_value)
ZAF_DEFINE_TYPE_END


class Derived2 : public Derived {
public:
    ZAF_DECLARE_TYPE

public:
    int derived_value2{};
};

class Derived2Praser : public zaf::ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, zaf::Object& object) override {

    }

    void ParseFromNode(const zaf::XamlNode& node, zaf::Object& object) override {

        auto base_value = node.GetAttribute(L"DerivedValue2");
        if (base_value) {
            dynamic_cast<Derived2&>(object).derived_value2 = zaf::ToNumeric<int>(*base_value);
        }
    }
};

ZAF_DEFINE_TYPE(Derived2)
ZAF_DEFINE_TYPE_PARSER(Derived2Praser)
ZAF_DEFINE_TYPE_END

}


TEST(ParsingTest, ParserLink) {

    auto xaml = LR"(<Derived DerivedValue="198" BaseValue="891" DerivedValue2="981" />)";
    auto reader = zaf::XamlReader::FromString(xaml);
    auto node = reader->Read();

    auto parser = Derived2::Type->GetParser();
    Derived2 object;
    parser->ParseFromNode(*node, object);

    ASSERT_EQ(object.base_value, 891);
    ASSERT_EQ(object.derived_value, 198);
    ASSERT_EQ(object.derived_value2, 981);
}