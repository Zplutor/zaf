#pragma once

#include <memory>
#include <zaf/base/rect.h>

namespace zaf {

class Canvas;
class Timer;
class Window;

class Caret {
public:
	Caret();
	~Caret();

	const Rect& GetRect() const {
		return rect_;
	}

	void SetRect(const Rect& rect);

	void SetPosition(const Point& position) {
		SetRect(Rect(position, GetRect().size));
	}

	void SetSize(const Size& size) {
		SetRect(Rect(GetRect().position, size));
	}

	void Show();
	void Hide();

private:
	friend class Window;

	void SetWindow(const std::shared_ptr<Window>& window) {
		window_ = window;
	}

	void Paint(Canvas& canvas);

private:
	bool IsShown() const {
		return blink_timer_ != nullptr;
	}

	void NeedRepaint(bool bling_visible);

private:
	std::weak_ptr<Window> window_;
	Rect rect_;
	bool is_visible_;
	std::unique_ptr<Timer> blink_timer_;
};

}