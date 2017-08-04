#pragma once

#include <zaf/graphic/frame.h>
#include "property/property_item.h"

std::shared_ptr<PropertyItem> CreateFramePropertyItem(
    const std::wstring& title, 
    const std::function<zaf::Frame()>& get_frame,
    const std::function<void(const zaf::Frame&)>& frame_change);