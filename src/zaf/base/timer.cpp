#include <zaf/base/timer.h>

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


Timer::Timer(Mode mode) :
    mode_(mode),
    interval_(MinimumInterval),
    is_running_(false) {

}


Timer::~Timer() {

	Stop();
}


void Timer::SetInterval(const Interval& interval) {

    interval_ = interval;

    if (IsRunning()) {
        StopSystemTimer();
        StartSystemTimer();
    }
}


void Timer::Start() {

	is_running_ = true;
    StartSystemTimer();
}


void Timer::Stop() {

    StopSystemTimer();
    is_running_ = false;
}


void Timer::StartSystemTimer() {

    CheckInitialize();

    SetTimer(
        g_timer_window,
        reinterpret_cast<UINT_PTR>(this),
        static_cast<UINT>(interval_.count()),
        Timer::TimerProcedure);
}


void Timer::StopSystemTimer() {

    KillTimer(g_timer_window, reinterpret_cast<UINT_PTR>(this));
}


void Timer::TimerProcedure(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {

	Timer* timer = reinterpret_cast<Timer*>(idEvent);
    auto shared_timer = timer->shared_from_this();
    shared_timer->SystemTimerTrigger();
}


void Timer::SystemTimerTrigger() {

    switch (GetMode()) {
        case Mode::OneShot:
            Stop();
            break;
        case Mode::DeferredRepeated:
            StopSystemTimer();
            break;
        default:
            break;
    }

    RaiseEvent();

    if (IsRunning() && (GetMode() == Mode::DeferredRepeated)) {
        StartSystemTimer();
    }
}


void Timer::RaiseEvent() {

    TimerTriggerInfo event_info;
    event_info.timer = shared_from_this();
    trigger_event_.GetObserver().OnNext(event_info);
}


const Timer::Interval Timer::MinimumInterval(std::chrono::milliseconds(USER_TIMER_MINIMUM));
const Timer::Interval Timer::MaximumInterval(std::chrono::milliseconds(USER_TIMER_MAXIMUM));

}