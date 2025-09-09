#pragma once

#include <zaf/dynamic/parsing/object_parser.h>

namespace zaf {

class Control;

/**
Parses a control's properties and children from XAML.
*/
class ControlParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override;

protected:
    /**
    Parses content nodes of the XAML node and sets the result to a specified control.

    @param nodes
        The content nodes to be parsed.

    @param control
        The control to which the parsed result will be set.

    @throw zaf::Error
        Thrown if there are content nodes that cannot be parsed to a control.

    @remark
        The default implementation parses each content node to a control and add them to the 
        specified control's children.

        Derived classes may override this method if they use content nodes for a different purpose.
    */
    virtual void ParseContentNodes(
        const std::vector<std::shared_ptr<XamlNode>>& nodes,
        Control& control);
};

}