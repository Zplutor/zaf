#include <zaf/control/textual/inline_object.h>
#include <zaf/base/as.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf::textual {

ZAF_DEFINE_TYPE(InlineObject)
ZAF_DEFINE_TYPE_END;

zaf::Size InlineObject::Size() const {
    auto metrics = this->GetMetrics();
    return zaf::Size{ metrics.Width(), metrics.Height() };
}


TextInlineObjectMetrics InlineObject::GetMetrics() const {

    TextInlineObjectMetrics result;
    result.SetWidth(16);
    result.SetHeight(16);
    result.SetHeightAboveBaseline(16);
    return result;
}


std::optional<Range> InlineObject::RangeInHost() const noexcept {

    if (host_.expired()) {
        return std::nullopt;
    }

    if (!attached_range_) {
        return std::nullopt;
    }

    return *attached_range_;
}


void InlineObject::OnAttached(const AttachedInfo& event_info) {
    attached_event_.Raise(event_info);
}


void InlineObject::OnDetached(const DetachedInfo& event_info) {
    detached_event_.Raise(event_info);
}


void InlineObject::Paint(Canvas& canvas) const {

    canvas.DrawRectangleFrame(
        Rect{ Point{}, this->Size() },
        1.f, 
        canvas.Renderer().CreateSolidColorBrush(Color::Black()));
}


void InlineObject::SetHost(std::shared_ptr<TextualControl> host) {

    ZAF_EXPECT(host);
    ZAF_EXPECT(attached_range_.has_value());
    ZAF_EXPECT(host_.expired());

    host_ = std::move(host);
    OnAttached(AttachedInfo{ As<InlineObject>(shared_from_this()) });
}


void InlineObject::Detach() {

    attached_range_.reset();

    if (!host_.expired()) {
        host_.reset();
        OnDetached(DetachedInfo{ As<InlineObject>(shared_from_this()) });
    }
}

}