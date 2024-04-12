#pragma once

#include <zaf/internal/control/control_focus_event_info_helpers.h>

namespace zaf {

using PreFocusGainedInfo = internal::TemplateFocusGainedInfo<__LINE__>;
using FocusGainedInfo = internal::TemplateFocusGainedInfo<__LINE__>;

using PreFocusLostInfo = internal::TemplateFocusLostInfo<__LINE__>;
using FocusLostInfo = internal::TemplateFocusLostInfo<__LINE__>;

}