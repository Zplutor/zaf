#include <gtest/gtest.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/control/control.h>
#include <zaf/object/object.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/xaml_reader.h>

namespace {

class Base : public zaf::Object {
public:
    ZAF_OBJECT;

public:
    int base_value{};
};

class BaseParser : public zaf::ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, zaf::Object& object) override {

    }

    void ParseFromNode(const zaf::XamlNode& node, zaf::Object& object) override {

        __super::ParseFromNode(node, object);

        auto base_value = node.FindAttribute(L"BaseValue");
        if (base_value) {
            dynamic_cast<Base&>(object).base_value = zaf::ToNumeric<int>(base_value->Value());
        }
    }
};

ZAF_OBJECT_BEGIN(Base);
ZAF_OBJECT_PARSER(BaseParser);
ZAF_OBJECT_END;
ZAF_OBJECT_IMPL(Base);


class Derived : public Base {
public:
    ZAF_OBJECT;

public:
    int DerivedValue() const {
        return derived_value;
    }

    void SetDerivedValue(int value) {
        derived_value = value;
    }

public:
    int derived_value{};
};

ZAF_OBJECT_BEGIN(Derived);
ZAF_PROPERTY(DerivedValue);
ZAF_OBJECT_END;
ZAF_OBJECT_IMPL(Derived);


class Derived2 : public Derived {
public:
    ZAF_OBJECT;

public:
    int derived_value2{};
};

class Derived2Praser : public BaseParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, zaf::Object& object) override {

    }

    void ParseFromNode(const zaf::XamlNode& node, zaf::Object& object) override {

        __super::ParseFromNode(node, object);

        auto base_value = node.FindAttribute(L"DerivedValue2");
        if (base_value) {
            dynamic_cast<Derived2&>(object).derived_value2 = 
                zaf::ToNumeric<int>(base_value->Value());
        }
    }
};

ZAF_OBJECT_BEGIN(Derived2)
ZAF_OBJECT_PARSER(Derived2Praser)
ZAF_OBJECT_END
ZAF_OBJECT_IMPL(Derived2);

}

TEST(ParsingTest, ParserLink) {

    {
        auto xaml = LR"(<Base BaseValue="891" />)";
        auto base = zaf::CreateObjectFromXaml<Base>(xaml);
        ASSERT_NE(base, nullptr);
        ASSERT_EQ(base->base_value, 891);
    }

    {
        auto xaml = LR"(<Derived DerivedValue="198" BaseValue="891" />)";
        auto derived = zaf::CreateObjectFromXaml<Derived>(xaml);
        ASSERT_NE(derived, nullptr);
        ASSERT_EQ(derived->base_value, 891);
        ASSERT_EQ(derived->derived_value, 198);
    }

    {
        auto xaml = LR"(<Derived2 DerivedValue="198" BaseValue="891" DerivedValue2="981" />)";
        auto derived2 = zaf::CreateObjectFromXaml<Derived2>(xaml);
        ASSERT_NE(derived2, nullptr);
        ASSERT_EQ(derived2->base_value, 891);
        ASSERT_EQ(derived2->derived_value, 198);
        ASSERT_EQ(derived2->derived_value2, 981);
    }
}


namespace {

class Host : public zaf::Object {
public:
    ZAF_OBJECT;

    std::shared_ptr<zaf::Control> Control() const {
        return control_;
    }

    void SetControl(const std::shared_ptr<zaf::Control>& control) {
        this->control_ = control;
    }

private:
    std::shared_ptr<zaf::Control> control_;
};

ZAF_OBJECT_BEGIN(Host)
ZAF_PROPERTY(Control);
ZAF_OBJECT_END
ZAF_OBJECT_IMPL(Host);

}

TEST(ParsingTest, ParseInvalidDynamicNode) {

    auto xaml = LR"(<Host><Host.Control><Child1/><Child2/></Host.Control></Host>)";
    auto node = zaf::XamlReader::FromString(xaml)->Read();

    auto parser = Host::Type->GetParser();
    Host host;
    ASSERT_THROW(parser->ParseFromNode(*node, host), zaf::ParseError);

    xaml = LR"(<Host><Host.Control></Host.Control></Host>)";
    node = zaf::XamlReader::FromString(xaml)->Read();
    ASSERT_THROW(parser->ParseFromNode(*node, host), zaf::ParseError);
}


namespace {

class PropertyNodeBase : public zaf::Object {
public:
    ZAF_OBJECT;

public:
    int BaseValue() const {
        return base_value;
    }

    void SetBaseValue(int value) {
        base_value = value;
    }

public:
    int base_value{};
};

ZAF_OBJECT_BEGIN(PropertyNodeBase)
ZAF_PROPERTY(BaseValue)
ZAF_OBJECT_END
ZAF_OBJECT_IMPL(PropertyNodeBase)

class PropertyNodeDerived : public PropertyNodeBase {
public:
    ZAF_OBJECT;
};

ZAF_OBJECT_BEGIN(PropertyNodeDerived)
ZAF_OBJECT_END
ZAF_OBJECT_IMPL(PropertyNodeDerived)

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

        ASSERT_EQ(object.base_value, 11);
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

        ASSERT_EQ(object.base_value, 12);
    }
}


namespace{

class PropertyOrderObject : public zaf::Object {
public:
    ZAF_OBJECT;

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

ZAF_OBJECT_BEGIN(PropertyOrderObject)
ZAF_PROPERTY(AProperty)
ZAF_PROPERTY(BProperty)
ZAF_OBJECT_END
ZAF_OBJECT_IMPL(PropertyOrderObject);

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