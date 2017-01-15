#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/property/textual_control_property_tag.h>

namespace zaf {

ZAF_DEFINE_EVENT_PROPERTY_TAG(onClick, ClickableControl::ClickEvent, ClickableControl, GetClickEvent);

}