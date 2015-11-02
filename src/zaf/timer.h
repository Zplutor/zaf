#pragma once

#include <Windows.h>
#include <cstdint>
#include <functional>

namespace zaf {

class Timer {
public:
	typedef std::function<void(Timer&)> Callback;

public:
	Timer(int interval_ms, const Callback& callback);
	~Timer();

	int GetInterval() const {
		return interval_;
	}

	void Start();
	void Stop();

private:
	static void CALLBACK TimerProcedure(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void Trigger();

private:
	int interval_;
	Callback callback_;
};

}