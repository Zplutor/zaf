#pragma once

#include <string>
#include <zaf/base/non_copyable.h>

namespace zaf {

namespace dynamic {
class Object;
}
class XamlNode;

/**
Provides a unified interface for parsing object's properties and contents from XAML.
*/
class ObjectParser : NonCopyableNonMovable {
public:
    ObjectParser() = default;
    virtual ~ObjectParser() = default;

    /**
    Parses a XAML attribute value and sets the result to a specified object.

    @param attribute_value
        The attribute value to be parsed.

    @param object
        The object to which the parsed result will be set.

    @remark
        If an object can be deserialized from a XAML attribute value, a corresponding derived class 
        can override this method to parse the string and set the result to the object. 

        Derived classes may throw exceptions if the attribute value is malformed.

        The default implementation of this method is empty.
    */
    virtual void ParseFromAttribute(const std::wstring& attribute_value, dynamic::Object& object);

    /**
    Parses a XAML node and sets the result to a specified object.

    @param node
        The XAML node to be parsed.

    @param object
        The object to which the parsed result will be set.

    @throw zaf::Error
        Thrown if there are malformed values in the XAML node that cannot be parsed or cannot be 
        set to the object.

    @remark
        The default implementation of this method parses attributes and property nodes of the XAML
        node and sets the values to the object's corresponding properties.

        Derived classes should override this method if they need to parse content nodes and should 
        call the default implementation to retain the ability of property parsing.
    */
    virtual void ParseFromNode(const XamlNode& node, dynamic::Object& object);

protected:
    virtual void ParsePropertyNode(
        const XamlNode& node,
        const std::wstring& property_name, 
        dynamic::Object& object);

private:
    void ParsePropertyNodes(const XamlNode& node, dynamic::Object& object);
    void ParseSinglePropertyNode(const XamlNode& node, dynamic::Object& object);
};

}