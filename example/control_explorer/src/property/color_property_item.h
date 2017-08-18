#pragma once

#include "property/property_item.h"

std::shared_ptr<PropertyItem> CreateColorPropertyItem(
    const std::wstring& title,
    const std::function<zaf::Color()>& get_color,
    const std::function<void(const zaf::Color&)>& color_change);