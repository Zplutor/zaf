#include <zaf/parsing/helpers.h>
#include <zaf/creation.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/creation.h>

namespace zaf {
namespace internal {

std::shared_ptr<ReflectionObject> CreateReflectionObjectFromXaml(
    const std::string& xaml,
    std::error_code& error_code) {

    auto xaml_reader = XamlReader::CreateFromString(xaml, error_code);
    if (error_code) {
        return {};
    }

    auto xaml_node = xaml_reader->Read(error_code);
    if (error_code) {
        return {};
    }

    auto object = CreateObjectByName(xaml_node->GetValue());
    if (!object) {
        return {};
    }

    object->GetType()->GetParser()->ParseFromNode(*xaml_node, *object);
    return object;
}

}
}