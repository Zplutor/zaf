#pragma once

#include "property_item.h"

std::shared_ptr<PropertyItem> CreateComboBoxPropertyItem(
    const std::wstring& title,
    const std::vector<std::wstring>& values,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change,
    const std::function<void(const std::function<void()>&)>& register_notification);

template<typename ValueType>
std::shared_ptr<PropertyItem> CreateComboBoxPropertyItem(
    const std::wstring& title,
    const std::vector<std::pair<std::wstring, ValueType>>& name_value_map,
    const std::function<ValueType()>& value_getter,
    const std::function<void(const ValueType&)>& value_setter,
    const std::function<void(const std::function<void()>&)>& register_notification) {

    std::vector<std::wstring> names;
    for (const auto& each_pair : name_value_map) {
        names.push_back(each_pair.first);
    }

    return CreateComboBoxPropertyItem(
        title,
        names,
        [name_value_map, value_getter]() -> std::wstring {
            auto value = value_getter();
            for (const auto& each_pair : name_value_map) {
                if (each_pair.second == value) {
                    return each_pair.first;
                }
            }
            return std::wstring();
        },
        [name_value_map, value_setter](const std::wstring& name) {
            for (const auto& each_pair : name_value_map) {
                if (each_pair.first == name) {
                    value_setter(each_pair.second);
                    break;
                }
            }
        },
        register_notification);
}