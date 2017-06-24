#include <zaf/graphic/size.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {

void Size::SerializeToDataNode(DataNode& data_node) const {
    
    data_node.AddChild(property::Width, DataNode::CreateNumber(width));
    data_node.AddChild(property::Height, DataNode::CreateNumber(height));
}


bool Size::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::Width) {
            width = data_node.GetFloat();
        }
        else if (key == property::Height) {
            height = data_node.GetFloat();
        }
    });

    return true;
}

ZAF_DEFINE_TYPE_NAME(Size);

}