#pragma once

#include <Unknwn.h>
#include <optional>
#include <zaf/base/com_object.h>
#include <zaf/base/non_copyable.h>
#include <zaf/control/rich_edit/object_context.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Canvas;
class RichEdit;

namespace rich_edit {

class EmbeddedObject : public NonCopyableNonMovable {
public:
    /**
    Tries to get EmbeddedObject from specified COM pointer.

    @param ptr
        The specified COM pointer, can be nullptr.

    @return
        Returns a pointer to EmbeddedObject if it is contained in the COM pointer. Returns nullptr
        if not.
    */
    static std::shared_ptr<EmbeddedObject> TryFromCOMPtr(const COMPtr<IUnknown>& ptr) noexcept;

public:
    EmbeddedObject() = default;
    virtual ~EmbeddedObject() = default;

    std::shared_ptr<RichEdit> Host() const {
        return host_.lock();
    }

    const zaf::Size& Size() const {
        return size_;
    }

    void SetSize(const zaf::Size& size) {
        size_ = size;
    }

    /**
    Gets character index of the object.

    @return
        Returns character index of the object. Returns std::nullopt if the object is not in a rich
        edit.

    @throw zaf::Error
        Thrown on failure.
    */
    std::optional<std::size_t> GetCharIndex() const;

    /**
    Gets position of the object in screen.

    @return
        Returns position of the object in screen. Returns std::nullopt if the object is not in a
        rich edit.

    @throw zaf::Error
        Thrown on failure.
    */
    std::optional<Point> GetPositionInScreen() const;

    virtual GUID ClassID() const = 0;

    /**
    Paints the embedded object. 

    @param canvas
        The canvas to which the object paints.

    @param dirty_rect
        A rectangle area that needs to be painted, in object coordindate.

    @param context
        An object to get context information of the object.
    */
    virtual void Paint(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const PaintContext& context) = 0;

    virtual void OnMouseCursorChanging(const MouseCursorChangingContext& context);
    virtual void OnMouseDown(const MouseDownContext& context);
    virtual void OnMouseUp(const MouseUpContext& context);

    //TODO: DoubleClickInfo should be a routed event so that there is IsHandled() to indicate that
    //the event is handled.
    virtual bool OnDoubleClick(const DoubleClickContext& context);

    void NeedRepaint();

private:
    friend class RichEdit;

    void SetHost(const std::weak_ptr<RichEdit>& host) {
        host_ = host;
    }

private:
    LONG reference_count_{ 1 };
    zaf::Size size_;
    std::weak_ptr<RichEdit> host_;
};

}
}