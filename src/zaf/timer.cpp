#include <zaf/timer.h>

namespace zaf {

static HWND g_timer_window = nullptr;

static void CheckInitialize() {

	if (g_timer_window != nullptr) {
		return;
	}

	g_timer_window = CreateWindow(
		L"STATIC",
		nullptr,
		0,
		0,
		0,
		0,
		0,
		HWND_MESSAGE,
		nullptr,
		nullptr,
		nullptr
	);
}


Timer::Timer(int interval_ms, const Callback& callback) :
	interval_(interval_ms),
	callback_(callback) {

}


Timer::~Timer() {

	Stop();
}


void Timer::Start() {

	CheckInitialize();

	SetTimer(
		g_timer_window, 
		reinterpret_cast<UINT_PTR>(this), 
		interval_, 
		Timer::TimerProcedure
	);
}


void Timer::Stop() {

	KillTimer(g_timer_window, reinterpret_cast<UINT_PTR>(this));
}


void Timer::TimerProcedure(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {

	Timer* timer = reinterpret_cast<Timer*>(idEvent);
	timer->Trigger();
}


void Timer::Trigger() {
	callback_(*this);
}


}