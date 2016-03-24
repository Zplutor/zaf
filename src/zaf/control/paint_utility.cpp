#include <zaf/control/paint_utility.h>
#include <zaf/control/control.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void PaintTextWithIcon(
	Canvas& canvas,
	const Rect& paint_rect,
	const std::shared_ptr<TextLayout>& text_layout,
	float icon_size,
	const std::function<void(Canvas& canvas, const Rect& text_rect, const std::shared_ptr<TextLayout>& text_layout)>& paint_text_function,
	const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function
) {

	Rect text_rect = paint_rect;
	float x_offset = icon_size + 5;
	text_rect.position.x += x_offset;
	text_rect.size.width -= x_offset;

	text_layout->SetMaxWidth(text_rect.size.width);
	text_layout->SetMaxHeight(text_rect.size.height);

	auto line_metrics = text_layout->GetLineMetrics(1);
	if (line_metrics.empty()) {
		return;
	}

	float icon_y = line_metrics.front().height - icon_size;
	if (icon_y != 0) {
		icon_y /= 2;
	}

	auto text_metrics = text_layout->GetMetrics();
	icon_y += text_metrics.top;

	Rect icon_rect(0, icon_y, icon_size, icon_size);
	paint_icon_function(canvas, icon_rect);
	paint_text_function(canvas, text_rect, text_layout);
}

}