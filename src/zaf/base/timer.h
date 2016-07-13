#pragma once

#include <Windows.h>
#include <chrono>
#include <memory>
#include <zaf/base/event.h>

namespace zaf {

/**
 Represent a timer.
 */
class Timer : public std::enable_shared_from_this<Timer> {
public:
    /**
     Defines the timer's working mode.
     */
    enum class Mode {

        /**
         The timer is repeated, which timeout begin counted after the previous trigger 
         event has been processed.
         */
        DeferredRepeated,

        /**
         The timer is repeated, which timeout begin counted immediately withou waiting 
         for processing the previous trigger event.
         */
        ImmediatelyRepeated,

        /**
         The timer is one-shot.
         */
        OneShot,
    };

    /**
     The type of interval.
     */
    typedef std::chrono::milliseconds Interval;

	/**
	 The type of trigger event.
	 */
    typedef Event<Timer&> TriggerEvent;

public:
    /**
     The minimum interval value.
     */
    static const Interval MinimumInterval;

    /**
     The maximum interval value.
     */
    static const Interval MaximumInterval;

public:
	/**
	 Initialize the timer that has specified working mode.

     The working mode is immutable after creating the timer.
     The interval has a minimum interval value by default.
	 */
	Timer(Mode mode);

	/**
	 Destroy the timer.

     The timer is stopped when it is destroyed.
	 */
	~Timer();

    Mode GetMode() const {
        return mode_;
    }

	/**
	 Get the timer's interval.
	 */
	const Interval GetInterval() const {
		return interval_;
	}

    /**
     Set the timer's interval.

     Setting the interval will restart the timer while it is running.
     */
    void SetInterval(const Interval& interval);

    /**
     Get the trigger event.
     */
    TriggerEvent::Proxy GetTriggerEvent() {
        return TriggerEvent::Proxy(trigger_event_);
    }

    /**
     Get a value indicating that whether the timer is running.
     */
    bool IsRunning() const {
        return is_running_;
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

    void StartSystemTimer();
    void StopSystemTimer();
	void SystemTimerTrigger();

private:
    Mode mode_;
	Interval interval_;
	TriggerEvent trigger_event_;
    bool is_running_;
};

}