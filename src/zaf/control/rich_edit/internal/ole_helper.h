#pragma once

#include <Richedit.h>
#include <richole.h>
#include <TOM.h>
#include <zaf/base/com_object.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/text_box.h>

namespace zaf::rich_edit::internal {

class OLEHelper {
public:
    struct ObjectInfo {
        COMObject<EmbeddedObject> object;
        std::size_t position{};
    };

    static ObjectInfo FindObjectUnderMouse(const TextBox& text_box);

private:
    struct TextRangeWithObject {
        COMObject<ITextRange> text_range;
        COMObject<EmbeddedObject> object;
    };

    static TextRangeWithObject FindTextRangeContainingObjectUnderMouse(
        const COMObject<IRichEditOle>& ole_interface,
        const POINT& mouse_position_in_screen);

    static COMObject<EmbeddedObject> GetObjectInTextRange(const COMObject<ITextRange>& text_range);
};

}