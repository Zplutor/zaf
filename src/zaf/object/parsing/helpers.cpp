#include <zaf/object/parsing/helpers.h>
#include <zaf/creation.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/creation.h>

namespace zaf {
namespace internal {

std::shared_ptr<Object> CreateReflectionObjectFromXaml(const std::string& xaml) {

    auto xaml_reader = XamlReader::FromString(xaml);
    auto xaml_node = xaml_reader->Read();

    auto object = CreateObjectByName(xaml_node->Value());
    if (!object) {
        return {};
    }

    object->DynamicType()->Parser()->ParseFromNode(*xaml_node, *object);
    return object;
}

}
}