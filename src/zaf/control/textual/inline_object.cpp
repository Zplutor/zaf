#include <zaf/control/textual/inline_object.h>
#include <zaf/base/as.h>
#include <zaf/graphic/canvas.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {

ZAF_OBJECT_IMPL(InlineObject);


zaf::Size InlineObject::Size() const {
    auto metrics = this->GetMetrics();
    return zaf::Size{ metrics.Width(), metrics.Height() };
}


dwrite::InlineObjectMetrics InlineObject::GetMetrics() const {

    dwrite::InlineObjectMetrics result;
    result.SetWidth(16);
    result.SetHeightAndBaseline(16);
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
    has_set_host_ = true;
    OnAttached(AttachedInfo{ As<InlineObject>(shared_from_this()) });
}


void InlineObject::Detach() {

    attached_range_.reset();

    //If the object has been attached to a host, DetachedEvent must be raised when the object is 
    //detached. 
    //We use an additional flag value has_set_host_ rather checking host.expired() to determine 
    //whether the object has been attached to a host, as when the object is detached due to the 
    //destruction of the host, host.expired() would be true.
    if (has_set_host_) {
        host_.reset();
        has_set_host_ = false;
        OnDetached(DetachedInfo{ As<InlineObject>(shared_from_this()) });
    }
}


std::shared_ptr<InlineObject> InlineObject::Clone() const {
    return As<InlineObject>(this->DynamicType()->CreateInstance());
}


void InlineObject::WriteToXML(XMLWriter& writer) const {
    writer.WriteElementStart(L"InlineObject");
    writer.WriteElementEnd();
}


void InlineObject::ReadFromXML(XMLReader& reader) {
    auto [is_empty] = reader.ReadElementStart(L"InlineObject");
    if (!is_empty) {
        reader.ReadElementEnd();
    }
}

}