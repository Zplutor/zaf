#pragma once

#include "property/property_item.h"

std::shared_ptr<PropertyItem> CreateTextPropertyItem(
    const std::wstring& title,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change);

std::shared_ptr<PropertyItem> CreateMultilineTextPropertyItem(
    const std::wstring& title,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change);