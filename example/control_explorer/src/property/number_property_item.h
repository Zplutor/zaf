#pragma once

#include "property/property_item.h"

std::shared_ptr<PropertyItem> CreateFloatPropertyItem(
    const std::wstring& title,
    const std::function<float()>& get_value,
    const std::function<void(float)>& value_change,
    const std::function<void(const std::function<void()>&)> register_notification);

inline std::shared_ptr<PropertyItem> CreateFloatPropertyItem(
    const std::wstring& title,
    const std::function<float()>& get_value,
    const std::function<void(float)>& value_change) {

    return CreateFloatPropertyItem(title, get_value, value_change, nullptr);
}


std::shared_ptr<PropertyItem> CreateIntegerPropertyItem(
    const std::wstring& title,
    const std::function<std::int64_t()>& get_value,
    const std::function<void(std::int64_t)>& value_change,
    const std::function<void(const std::function<void()>&)> register_notification);

inline std::shared_ptr<PropertyItem> CreateIntegerPropertyItem(
    const std::wstring& title,
    const std::function<std::int64_t()>& get_value,
    const std::function<void(std::int64_t)>& value_change) {

    return CreateIntegerPropertyItem(title, get_value, value_change, nullptr);
}