#pragma once

#include "property/property_item.h"

std::shared_ptr<PropertyItem> CreateFloatPropertyItem(
    const std::wstring& title,
    const std::function<float()>& get_value,
    const std::function<void(float)>& value_change);

std::shared_ptr<PropertyItem> CreateIntegerPropertyItem(
    const std::wstring& title,
    const std::function<std::int64_t()>& get_value,
    const std::function<void(std::int64_t)>& value_change);