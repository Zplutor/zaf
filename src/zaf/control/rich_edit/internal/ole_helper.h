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

    static ObjectInfo FindObjectAtScreenPosition(
        const RichEdit& rich_edit, 
        const POINT& position_in_pixels);

private:
    struct InnerObjectInfo {
        COMObject<EmbeddedObject> object;
        int object_x{};
        int object_y{};
        long text_position{};
    };

    static std::optional<InnerObjectInfo> InnerFindObjectAtPosition(
        const RichEdit& rich_edit,
        const POINT& position_in_screen);

    static std::optional<InnerObjectInfo> GetObjectInTextRange(
        const RichEdit& rich_edit,
        const COMObject<ITextRange>& text_range,
        const POINT& position_in_screen);
};

}