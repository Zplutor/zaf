#include <zaf/control/rich_edit/internal/ole_helper.h>
#include <zaf/graphic/dpi.h>

namespace zaf::rich_edit::internal {

OLEHelper::ObjectInfo OLEHelper::FindObjectUnderMouse(const TextBox& text_box) {

    POINT mouse_position{};
    if (!GetCursorPos(&mouse_position)) {
        return {};
    }

    auto [text_range, object] = FindTextRangeContainingObjectUnderMouse(
        text_box.GetOLEInterface(),
        mouse_position);

    if (!text_range || !object) {
        return {};
    }

    //Note: the object found from rich edit is not always under mouse cursor, so we double check to 
    //make sure it is.

    long object_x{};
    long object_y{};
    HRESULT hresult = text_range->GetPoint(tomStart | TA_TOP | TA_LEFT, &object_x, &object_y);
    if (FAILED(hresult)) {
        return {};
    }

    auto object_size = FromDIPs(object->Size(), text_box.GetDPI()).ToSIZE();

    RECT object_rect;
    object_rect.left = object_x;
    object_rect.top = object_y;
    object_rect.right = object_x + object_size.cx;
    object_rect.bottom = object_y + object_size.cy;

    if (!PtInRect(&object_rect, mouse_position)) {
        return {};
    }

    long object_position{};
    hresult = text_range->GetStart(&object_position);
    if (FAILED(hresult)) {
        return {};
    }

    ObjectInfo result;
    result.object = object;
    result.position = static_cast<std::size_t>(object_position);
    return result;
}


OLEHelper::TextRangeWithObject OLEHelper::FindTextRangeContainingObjectUnderMouse(
    const COMObject<IRichEditOle>& ole_interface,
    const POINT& mouse_position_in_screen) {

    auto text_document = ole_interface.As<ITextDocument>();
    if (!text_document) {
        return {};
    }

    COMObject<ITextRange> text_range;
    HRESULT hresult = text_document->RangeFromPoint(
        mouse_position_in_screen.x,
        mouse_position_in_screen.y,
        text_range.Store());

    if (FAILED(hresult)) {
        return {};
    }

    auto object = GetObjectInTextRange(text_range);
    if (object) {
        TextRangeWithObject result;
        result.text_range = text_range;
        result.object = object;
        return result;
    }

    //Note: We will probably get a wrong text range which is next to the object even if the 
    //mouse is just right above the object. So we check the previsous text range again to find the 
    //object.
    long text_range_position{};
    hresult = text_range->GetStart(&text_range_position);
    if (FAILED(hresult)) {
        return {};
    }

    if (text_range_position <= 0) {
        return {};
    }

    auto revised_position = text_range_position - 1;
    hresult = text_document->Range(revised_position, revised_position, text_range.Store());
    if (FAILED(hresult)) {
        return {};
    }

    object = GetObjectInTextRange(text_range);
    if (object) {
        TextRangeWithObject result;
        result.text_range = text_range;
        result.object = object;
        return result;
    }

    return {};
}


COMObject<EmbeddedObject> OLEHelper::GetObjectInTextRange(
    const COMObject<ITextRange>& text_range) {

    COMObject<IUnknown> ole_object;
    HRESULT hresult = text_range->GetEmbeddedObject(ole_object.Store());
    if (FAILED(hresult)) {
        return {};
    }

    auto embedded_object = dynamic_cast<rich_edit::EmbeddedObject*>(ole_object.Inner());
    if (!embedded_object) {
        return {};
    }

    embedded_object->AddRef();
    COMObject<rich_edit::EmbeddedObject> result{ embedded_object };
    return result;
}

}