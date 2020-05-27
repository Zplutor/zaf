#include <zaf/parsing/helpers.h>
#include <zaf/creation.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/creation.h>

namespace zaf {
namespace internal {

std::shared_ptr<ReflectionObject> CreateReflectionObjectFromXaml(const std::string& xaml) {

    auto xaml_reader = XamlReader::CreateFromString(xaml);
    auto xaml_node = xaml_reader->Read();

    auto object = CreateObjectByName(xaml_node->GetValue());
    if (!object) {
        return {};
    }

    object->GetType()->GetParser()->ParseFromNode(*xaml_node, *object);
    return object;
}

}
}