#include <zaf/control/rich_edit/internal/ole_helper.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

namespace zaf::rich_edit::internal {

OLEHelper::ObjectInfo OLEHelper::FindObjectUnderMouse(const RichEdit& rich_edit) {

    POINT mouse_position{};
    if (!GetCursorPos(&mouse_position)) {
        return {};
    }

    return FindObjectAtScreenPosition(rich_edit, mouse_position);
}


OLEHelper::ObjectInfo OLEHelper::FindObjectAtScreenPosition(
    const RichEdit& rich_edit, 
    const POINT& position_in_pixels) {

    auto inner_object_info = InnerFindObjectAtPosition(rich_edit, position_in_pixels);
    if (!inner_object_info) {
        return {};
    }

    ObjectInfo result;
    result.object = inner_object_info->object;
    result.text_position = static_cast<std::size_t>(inner_object_info->text_position);
    result.is_in_selection_range = rich_edit.GetSelectionRange().Contain(result.text_position);

    POINT mouse_position_in_object = position_in_pixels;
    mouse_position_in_object.x -= inner_object_info->object_x;
    mouse_position_in_object.y -= inner_object_info->object_y;
    result.mouse_position_in_object = ToDIPs(
        Point::FromPOINT(mouse_position_in_object), 
        rich_edit.GetDPI());

    result.object_position_in_screen = ToDIPs(
        Point{ 
            static_cast<float>(inner_object_info->object_x), 
            static_cast<float>(inner_object_info->object_y) 
        },
        rich_edit.GetDPI());

    return result;
}


std::optional<OLEHelper::InnerObjectInfo> OLEHelper::InnerFindObjectAtPosition(
    const RichEdit& rich_edit,
    const POINT& position_in_screen) {

    auto text_document = rich_edit.GetOLEInterface().Inner().Query<ITextDocument>();
    if (!text_document) {
        return {};
    }

    COMPtr<ITextRange> text_range;
    HRESULT hresult = text_document->RangeFromPoint(
        position_in_screen.x,
        position_in_screen.y,
        text_range.Reset());

    if (FAILED(hresult)) {
        return {};
    }

    long text_range_position{};
    hresult = text_range->GetStart(&text_range_position);
    if (FAILED(hresult)) {
        return {};
    }

    auto object_info = GetObjectInTextRange(rich_edit, text_range, position_in_screen);
    if (object_info) {
        object_info->text_position = text_range_position;
        return object_info;
    }

    //Note: We will probably get a wrong text range which is next to the object even if the 
    //specified position is just right above the object. So we check the previsous text range once
    //more to find the object.
    if (text_range_position <= 0) {
        return {};
    }

    auto revised_position = text_range_position - 1;
    hresult = text_document->Range(revised_position, revised_position, text_range.Reset());
    if (FAILED(hresult)) {
        return {};
    }

    object_info = GetObjectInTextRange(rich_edit, text_range, position_in_screen);
    if (object_info) {
        object_info->text_position = revised_position;
        return object_info;
    }

    return {};
}


std::optional<OLEHelper::InnerObjectInfo> OLEHelper::GetObjectInTextRange(
    const RichEdit& rich_edit,
    const COMPtr<ITextRange>& text_range,
    const POINT& position_in_screen) {

    COMPtr<IUnknown> ole_object;
    HRESULT hresult = text_range->GetEmbeddedObject(ole_object.Reset());
    if (FAILED(hresult)) {
        return {};
    }

    //GetEmbeddedObject() may return S_FALSE and null in some cases.
    if (!ole_object) {
        return {};
    }

    auto object = EmbeddedObject::TryFromCOMPtr(ole_object);
    if (!object) {
        return {};
    }

    //Note: the object found from rich edit is not always under the specified position (which
    //might be a bug of rich edit), so we double check to make sure it is.
    long object_x{};
    long object_y{};
    hresult = text_range->GetPoint(tomStart | TA_TOP | TA_LEFT, &object_x, &object_y);
    if (FAILED(hresult)) {
        return {};
    }

    auto object_size = FromDIPs(object->Size(), rich_edit.GetDPI()).ToSIZE();
    if (rich_edit.IsMultiline()) {
        //For multi-line rich edit, we check if the specified position is in the whole rectangle 
        //area of the object.
        RECT object_rect;
        object_rect.left = object_x;
        object_rect.top = object_y;
        object_rect.right = object_x + object_size.cx;
        object_rect.bottom = object_y + object_size.cy;
        if (!PtInRect(&object_rect, position_in_screen)) {
            return {};
        }
    }
    else {
        //For single-line rich edit, we just check if the specified position is in the horizontal 
        //range of the object.
        if ((position_in_screen.x < object_x) ||
            (position_in_screen.x >= object_x + object_size.cx)) {
            return {};
        }
    }

    InnerObjectInfo result;
    result.object = object;
    result.object_x = object_x;
    result.object_y = object_y;
    return result;
}

}