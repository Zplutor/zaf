#pragma once

#include <Windows.h>
#include <vector>
#include "../rect.h"

namespace zaf {
namespace internal {

class WindowState {
public:
	virtual ~WindowState() { }

	virtual const Rect GetRect() const = 0;
	virtual void SetRect(const Rect& rect) = 0;

	virtual const std::wstring GetTitle() const = 0;
	virtual void SetTitle(const std::wstring& title) = 0;
};


class WindowNotCreatedState : public WindowState {
public:
	const Rect GetRect() const override {
		return rect_;
	}

	void SetRect(const Rect& rect) override {
		rect_ = rect;
	}

	const std::wstring GetTitle() const {
		return title_;
	}

	void SetTitle(const std::wstring& title) {
		title_ = title;
	}

private:
	Rect rect_;
	std::wstring title_;
};


class WindowCreatedState : public WindowState {
public:
	WindowCreatedState(HWND window_handle) :
		window_handle_(window_handle) {
	
	}


	const Rect GetRect() const override {
		RECT rect = { 0 };
		GetWindowRect(window_handle_, &rect);
		return Rect(
			static_cast<float>(rect.left), 
			static_cast<float>(rect.top), 
			static_cast<float>(rect.left + rect.right), 
			static_cast<float>(rect.top + rect.bottom)
		);
	}


	void SetRect(const Rect& rect) override {

		SetWindowPos(
			window_handle_,
			NULL,
			static_cast<int>(rect.position.x),
			static_cast<int>(rect.position.y),
			static_cast<int>(rect.size.width),
			static_cast<int>(rect.size.height),
			SWP_NOZORDER
		);
	}


	const std::wstring GetTitle() const {

		int title_length = GetWindowTextLength(window_handle_);
		std::vector<wchar_t> buffer(title_length + 1);
		GetWindowText(window_handle_, buffer.data(), buffer.size());
		return buffer.data();
	}


	void SetTitle(const std::wstring& title) {
		SetWindowText(window_handle_, title.c_str());
	}

private:
	HWND window_handle_;
};

}
}