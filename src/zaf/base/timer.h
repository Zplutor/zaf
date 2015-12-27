#pragma once

#include <Windows.h>
#include <functional>

namespace zaf {

/**
 Represent a repeating timer.

 An interval, in milliseconds, and a callback are required to create a timer.
 A new-created timer is stopped by default, and the Start method can be used to start it.
 The Stop method is used to stop the timer.

 Once the timer starts, it triggers at every interval, and its callback would be called.

 Timer is immutable, thus its interval and callback cannot be changed.
 */
class Timer {
public:
	/**
	 Interval type.
	 */
	typedef std::size_t Interval;

	/**
	 Callback prototype.
	 */
	typedef std::function<void(Timer&)> Callback;

public:
	/**
	 Initialize the timer that has specified interval and callback.

	 callback can be nullptr, means doing nothing when the timer triggers.
	 */
	Timer(Interval interval, const Callback& callback);

	/**
	 Destroy the timer.
	 */
	~Timer();

	/**
	 Get the timer interval.
	 */
	Interval GetInterval() const {
		return interval_;
	}

	/**
	 Start the timer.
	 */
	void Start();

	/**
	 Stop the timer.
	 */
	void Stop();

private:
	static void CALLBACK TimerProcedure(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void Trigger();

private:
	Interval interval_;
	Callback callback_;
};

}