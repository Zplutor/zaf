#include <zaf/control/rich_edit/embedded_object.h>
#include <TOM.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/control/rich_edit.h>
#include <zaf/control/rich_edit/internal/ole_object_impl.h>
#include <zaf/graphic/dpi.h>

namespace zaf::rich_edit {
namespace {

Point GetObjectPositionInScreenInPixels(
    const OLEInterface& ole_interface, 
    std::size_t char_index) {

    auto text_document = ole_interface.Inner().Query<ITextDocument>();
    if (!text_document) {
        return {};
    }

    COMPtr<ITextRange> text_range;
    HRESULT hresult = text_document->Range(
        static_cast<long>(char_index), 
        static_cast<long>(char_index + 1), 
        text_range.Reset());

    ZAF_THROW_IF_COM_ERROR(hresult);

    long object_x{};
    long object_y{};
    hresult = text_range->GetPoint(tomStart | TA_TOP | TA_LEFT, &object_x, &object_y);
    ZAF_THROW_IF_COM_ERROR(hresult);

    return Point{ static_cast<float>(object_x), static_cast<float>(object_y) };
}

}


std::shared_ptr<EmbeddedObject> EmbeddedObject::TryFromCOMPtr(
    const COMPtr<IUnknown>& ptr) noexcept {

    auto ole_object_impl = ptr.As<internal::OLEObjectImpl>();
    if (ole_object_impl) {
        return ole_object_impl->EmbeddedObject();
    }
    return nullptr;
}


std::optional<std::size_t> EmbeddedObject::GetCharIndex() const {

    auto host = Host();
    if (!host) {
        return std::nullopt;
    }

    auto ole_interface = host->GetOLEInterface();

    //Loop over all objects to find index of current object.
    for (auto index : zaf::Range(0, ole_interface.GetObjectCount())) {

        auto object_info = ole_interface.GetObjectInfoAt(index);

        auto embedded_object = EmbeddedObject::TryFromCOMPtr(object_info.Object());
        if (embedded_object.get() == this) {
            return object_info.Index();
        }
    }

    return std::nullopt;
}


std::optional<Point> EmbeddedObject::GetPositionInScreen() const {

    auto host = Host();
    if (!host) {
        return std::nullopt;
    }

    auto object_index = GetCharIndex();
    if (!object_index) {
        return std::nullopt;
    }

    auto ole_interface = host->GetOLEInterface();
    auto position_in_pixels = GetObjectPositionInScreenInPixels(ole_interface, *object_index);
    return ToDIPs(position_in_pixels, host->GetDPI());
}


void EmbeddedObject::OnMouseCursorChanging(const MouseCursorChangingContext& context) {

}


void EmbeddedObject::OnMouseDown(const MouseDownContext& context) {

}


void EmbeddedObject::OnMouseUp(const MouseUpContext& context) {

}


bool EmbeddedObject::OnDoubleClick(const DoubleClickContext& context) {
    return false;
}


void EmbeddedObject::NeedRepaint() {

    auto host = Host();
    if (host) {
        host->NeedRepaint();
    }
}

}