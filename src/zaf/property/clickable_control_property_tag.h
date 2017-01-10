#pragma once

#include <zaf/control/button.h>
#include <zaf/property/property_tag.h>

namespace zaf {

ZAF_DEFINE_EVENT_PROPERTY_TAG(onClick, ClickableControl::ClickEvent, ClickableControl, GetClickEvent);

}