#include <zaf/parsing/helpers.h>
#include <zaf/creation.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>

namespace zaf {

void ParseObjectFromXaml(
    const std::string& xaml,
    ReflectionObject& object,
    std::error_code& error_code) {

    auto xaml_reader = XamlReader::CreateFromString(xaml, error_code);
    if (error_code) {
        return;
    }

    auto xaml_node = xaml_reader->Read(error_code);
    if (error_code) {
        return;
    }

    object.GetType()->GetParser()->ParseFromNode(*xaml_node, object);
}

void ParseObjectFromXaml(
    const std::wstring& xaml,
    ReflectionObject& object, 
    std::error_code& error_code) {

    ParseObjectFromXaml(ToUtf8String(xaml), object, error_code);
}




}