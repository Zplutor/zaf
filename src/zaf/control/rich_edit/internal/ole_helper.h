#pragma once

#include <Richedit.h>
#include <richole.h>
#include <TOM.h>
#include <zaf/base/com_object.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/rich_edit.h>

namespace zaf::rich_edit::internal {

class OLEHelper {
public:
    struct ObjectInfo {
        COMObject<EmbeddedObject> object;
        std::size_t text_position{};
        bool is_in_selection_range{};
        Point mouse_position_in_object;
        Point object_position_in_screen;
    };

    static ObjectInfo FindObjectUnderMouse(const RichEdit& rich_edit);

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