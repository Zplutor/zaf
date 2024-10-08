#pragma once

#include <zaf/creation.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/object_type.h>

namespace zaf {

class XamlNodeParseHelper {
public:
    XamlNodeParseHelper(const XamlNode& node, ObjectType* object_type);
    XamlNodeParseHelper(const XamlNodeParseHelper&) = delete;
    XamlNodeParseHelper& operator=(const XamlNodeParseHelper&) = delete;

    std::optional<std::wstring> GetContentString() const;

    std::optional<std::wstring> GetStringProperty(const std::wstring& property_name) const;
    std::optional<bool> GetBoolProperty(const std::wstring& property_name) const;
    std::optional<float> GetFloatProperty(const std::wstring& property_name) const;

    template<typename T>
    std::shared_ptr<T> GetObjectProperty(const std::wstring& property_name) const {

        auto property_node = GetPropertyNode(property_name);
        if (property_node) {
            auto object = Create<T>();
            T::Type->Parser()->ParseFromNode(*property_node, *object);
            return object;
        }

        auto attribute = node_.FindAttribute(property_name);
        if (attribute) {
            auto object = Create<T>();
            T::Type->Parser()->ParseFromAttribute(attribute->Value(), *object);
            return object;
        }

        return {};
    }

    template<typename T>
    std::shared_ptr<T> GetDynamicObjectProperty(const std::wstring& property_name) const {

        auto property_node = GetPropertyNode(property_name);
        if (property_node) {

            const auto& content_nodes = property_node->GetContentNodes();
            if (content_nodes.size() != 1) {
                return {};
            }

            return internal::CreateObjectFromNode<T>(content_nodes.front());
        }

        auto attribute = node_.FindAttribute(property_name);
        if (!attribute) {
            return {};
        }

        return CreateObjectByName<T>(attribute->Value());
    }

private:
    std::optional<std::wstring> GetStringFromPropertyNode(const std::wstring& name) const;
    std::shared_ptr<XamlNode> GetPropertyNode(const std::wstring& property_name) const;

private:
    const XamlNode& node_;
    ObjectType* object_type_{};
};

}