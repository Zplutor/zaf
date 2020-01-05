#pragma once

#include <zaf/creation.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

class XamlNodeParseHelper {
public:
    XamlNodeParseHelper(const XamlNode& node, const std::shared_ptr<ReflectionType>& object_type);
    XamlNodeParseHelper(const XamlNodeParseHelper&) = delete;
    XamlNodeParseHelper& operator=(const XamlNodeParseHelper&) = delete;

    std::optional<std::wstring> GetString(const std::wstring& property_name) const;
    std::optional<bool> GetBool(const std::wstring& property_name) const;
    std::optional<float> GetFloat(const std::wstring& property_name) const;

    template<typename T>
    std::shared_ptr<T> GetObjectAsPointer(const std::wstring& property_name) const {

        auto property_node = GetPropertyNode(property_name);
        if (property_node) {
            auto object = Create<T>();
            T::Type->GetParser()->ParseFromNode(property_node, *object);
            return object;
        }

        auto attribute = node_.GetAttribute(property_name);
        if (attribute) {
            auto object = Create<T>();
            T::Type->GetParser()->ParseFromAttribute(*attribute, *object);
            return object;
        }

        return {};
    }

private:
    std::optional<std::wstring> GetStringFromPropertyNode(const std::wstring& name) const;
    std::shared_ptr<XamlNode> GetPropertyNode(const std::wstring& property_name) const;

private:
    const XamlNode& node_;
    std::shared_ptr<ReflectionType> object_type_;
};

}