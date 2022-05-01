#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/control/control.h>
#include <zaf/object/object.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/type_definition.h>
#include "utility/assert.h"

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

        auto base_value = node.FindAttribute(L"BaseValue");
        if (base_value) {
            dynamic_cast<Base&>(object).base_value = zaf::ToNumeric<int>(base_value->Value());
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

        auto base_value = node.FindAttribute(L"DerivedValue2");
        if (base_value) {
            dynamic_cast<Derived2&>(object).derived_value2 = 
                zaf::ToNumeric<int>(base_value->Value());
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


namespace {

class Host : public zaf::Object {
public:
    ZAF_DECLARE_TYPE

    std::shared_ptr<zaf::Control> Control() const {
        return control_;
    }

    void SetControl(const std::shared_ptr<zaf::Control>& control) {
        this->control_ = control;
    }

private:
    std::shared_ptr<zaf::Control> control_;
};

ZAF_DEFINE_TYPE(Host)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(Control);
ZAF_DEFINE_TYPE_END

}

TEST(ParsingTest, ParseInvalidDynamicNode) {

    auto xaml = LR"(<Host><Host.Control><Child1/><Child2/></Host.Control></Host>)";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    auto parser = Host::Type->GetParser();
    Host host;
    ASSERT_THROW_ERRC(parser->ParseFromNode(*node, host), zaf::BasicErrc::InvalidValue);

    xaml = LR"(<Host><Host.Control></Host.Control></Host>)";
    node = zaf::XamlReader::FromString(xaml)->Read();
    ASSERT_THROW_ERRC(parser->ParseFromNode(*node, host), zaf::BasicErrc::InvalidValue);
}


namespace {

class PropertyNodeBase : public zaf::Object {
public:
    ZAF_DECLARE_TYPE

    int BaseValue{};
};

ZAF_DEFINE_TYPE(PropertyNodeBase)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(BaseValue, BaseValue)
ZAF_DEFINE_TYPE_END

class PropertyNodeDerived : public PropertyNodeBase {
public:
    ZAF_DECLARE_TYPE
};

ZAF_DEFINE_TYPE(PropertyNodeDerived)
ZAF_DEFINE_TYPE_END

}

TEST(ParsingTest, ParsePropertyNode) {

    //Property with derived class name.
    {
        auto xaml = LR"(
            <PropertyNodeDerived>
                <PropertyNodeDerived.BaseValue>11</PropertyNodeDerived.BaseValue>
            </PropertyNodeDerived>
        )";
        auto node = zaf::XamlReader::FromString(xaml)->Read();

        auto parser = PropertyNodeDerived::Type->GetParser();
        PropertyNodeDerived object;
        parser->ParseFromNode(*node, object);

        ASSERT_EQ(object.BaseValue, 11);
    }

    //Property with base class name.
    {
        auto xaml = LR"(
            <PropertyNodeDerived>
                <PropertyNodeBase.BaseValue>12</PropertyNodeBase.BaseValue>
            </PropertyNodeDerived>
        )";
        auto node = zaf::XamlReader::FromString(xaml)->Read();

        auto parser = PropertyNodeDerived::Type->GetParser();
        PropertyNodeDerived object;
        parser->ParseFromNode(*node, object);

        ASSERT_EQ(object.BaseValue, 12);
    }
}


namespace{

class PropertyOrderObject : public zaf::Object {
public:
    ZAF_DECLARE_TYPE

    void SetAProperty(int value) {
        a_property_order_ = ++order_seed_;
    }

    void SetBProperty(int value) {
        b_property_order_ = ++order_seed_;
    }

    int APropertyOrder() const {
        return a_property_order_;
    }

    int BProeprtyOrder() const {
        return b_property_order_;
    }

private:
    int order_seed_{};
    int a_property_order_{};
    int b_property_order_{};
};

ZAF_DEFINE_TYPE(PropertyOrderObject)
ZAF_DEFINE_TYPE_PROPERTY(AProperty)
ZAF_DEFINE_TYPE_PROPERTY(BProperty)
ZAF_DEFINE_TYPE_END

}

TEST(ParsingTest, ParsePropertyOrder) {

    {
        auto xaml = LR"(<PropertyOrderObject BProperty="2" AProperty="1" />)";
        auto node = zaf::XamlReader::FromString(xaml)->Read();

        PropertyOrderObject object;
        PropertyOrderObject::Type->GetParser()->ParseFromNode(*node, object);

        ASSERT_GT(object.APropertyOrder(), object.BProeprtyOrder());
    }

    {
        auto xaml = LR"(
            <PropertyOrderObject>
                <PropertyOrderObject.BProperty>4</PropertyOrderObject.BProperty>
                <PropertyOrderObject.AProperty>3</PropertyOrderObject.AProperty>
            </PropertyOrderObject>
        )";
        auto node = zaf::XamlReader::FromString(xaml)->Read();

        PropertyOrderObject object;
        PropertyOrderObject::Type->GetParser()->ParseFromNode(*node, object);

        ASSERT_GT(object.APropertyOrder(), object.BProeprtyOrder());
    }
}